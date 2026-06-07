/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 19:17:38 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/08 00:02:30 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder)
{
	long long	p;

	if (coder->data->scheduler_type != EDF)
		return (get_time_ms());
	pthread_mutex_lock(&coder->data->sim_mutex);
	p = coder->last_compile_start + coder->data->time_to_burnout;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	return (p);
}

static int	acquire_single_dongle(t_dongle *dongle, t_coder *coder, \
long long priority)
{
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->heap, coder->id, priority);
	while (1)
	{
		if (check_sim_over(coder->data))
		{
			heap_remove(&dongle->heap, coder->id);
			return (pthread_mutex_unlock(&dongle->mutex), 0);
		}
		if (dongle->is_available && dongle->heap.nodes[0].coder_id == coder->id)
		{
			if (get_time_ms() >= dongle->cooldown_end)
				break ;
		}
		pthread_mutex_unlock(&dongle->mutex);
		usleep(500);
		pthread_mutex_lock(&dongle->mutex);
	}
	heap_remove(&dongle->heap, coder->id);
	dongle->is_available = 0;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

static void	get_dongle_order(t_coder *coder, t_dongle **first, \
t_dongle **second)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*first = coder->left_dongle;
		*second = coder->right_dongle;
	}
	else
	{
		*first = coder->right_dongle;
		*second = coder->left_dongle;
	}
}

void	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;
	long long	priority;

	if (coder->data->nb_coders == 1)
		return ;
	priority = get_priority(coder);
	get_dongle_order(coder, &first, &second);
	if (!acquire_single_dongle(first, coder, priority))
		return ;
	if (!acquire_single_dongle(second, coder, priority))
	{
		pthread_mutex_lock(&first->mutex);
		heap_remove(&first->heap, coder->id);
		first->is_available = 1;
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
