/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 19:17:38 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/05 15:40:40 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder)
{
	long long	wait_time;

	if (coder->data->scheduler_type != EDF)
		return (get_time_ms());
	if (coder->compiles_count == 0)
		return (get_time_ms());
	wait_time = get_time_ms() - coder->last_compile_start;
	return (coder->last_compile_start - wait_time);
}

static int	acquire_single_dongle(t_dongle *dongle, t_coder *coder)
{
	long long	priority;

	priority = get_priority(coder);
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->heap, coder->id, priority);
	while (1)
	{
		if (check_sim_over(coder->data))
			return (pthread_mutex_unlock(&dongle->mutex), 0);
		if (dongle->is_available
			&& dongle->heap.nodes[0].coder_id == coder->id)
			if (get_time_ms() >= dongle->cooldown_end)
				break ;
		pthread_mutex_unlock(&dongle->mutex);
		usleep(500);
		pthread_mutex_lock(&dongle->mutex);
	}
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

void	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	if (!acquire_single_dongle(first, coder))
		return ;
	if (!acquire_single_dongle(second, coder))
	{
		pthread_mutex_lock(&first->mutex);
		first->is_available = 1;
		heap_pop(&first->heap);
		pthread_cond_broadcast(&first->cond);
		pthread_mutex_unlock(&first->mutex);
	}
}

void	release_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	first = coder->left_dongle;
	if (coder->left_dongle->id > coder->right_dongle->id)
		first = coder->right_dongle;
	second = coder->right_dongle;
	if (first == coder->right_dongle)
		second = coder->left_dongle;
	unlock_dongle(first, coder->data);
	if (first != second)
		unlock_dongle(second, coder->data);
}
