/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 19:17:38 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/09 23:16:09 by guifouqu         ###   ########.fr       */
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

static int	wait_for_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	while (!(dongle->is_available
			&& dongle->heap.nodes[0].coder_id == coder->id
			&& get_time_ms() >= dongle->cooldown_end))
	{
		if (check_sim_over(coder->data))
		{
			heap_remove(&dongle->heap, coder->id);
			return (pthread_mutex_unlock(&dongle->mutex), 0);
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

static void	prepare_dongles(t_coder *coder, t_dongle **f, t_dongle **s,
				long long p)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		*f = coder->left_dongle;
		*s = coder->right_dongle;
	}
	else
	{
		*f = coder->right_dongle;
		*s = coder->left_dongle;
	}
	pthread_mutex_lock(&(*f)->mutex);
	heap_push(&(*f)->heap, coder->id, p);
	pthread_mutex_unlock(&(*f)->mutex);
	pthread_mutex_lock(&(*s)->mutex);
	heap_push(&(*s)->heap, coder->id, p);
	pthread_mutex_unlock(&(*s)->mutex);
}

void	acquire_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->data->nb_coders == 1)
		return ;
	prepare_dongles(coder, &first, &second, get_priority(coder));
	if (!wait_for_dongle(first, coder))
	{
		pthread_mutex_lock(&second->mutex);
		heap_remove(&second->heap, coder->id);
		pthread_mutex_unlock(&second->mutex);
		return ;
	}
	print_status(coder, "has taken a dongle");
	if (!wait_for_dongle(second, coder))
	{
		pthread_mutex_lock(&first->mutex);
		first->is_available = 1;
		pthread_mutex_unlock(&first->mutex);
		return ;
	}
	print_status(coder, "has taken a dongle");
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
