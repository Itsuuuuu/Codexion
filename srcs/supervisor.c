/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   supervisor.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/07 22:39:48 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/09 14:03:58 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	launch_threads(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		pthread_mutex_lock(&data->sim_mutex);
		data->coders[i].last_compile_start = get_time_ms();
		pthread_mutex_unlock(&data->sim_mutex);
		if (pthread_create(&data->coders[i].thread_id, NULL, \
&coder_routine, &data->coders[i]) != 0)
			return (0);
		i++;
	}
	return (1);
}

static int	check_all_burnouts(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->nb_coders)
	{
		if (check_burnout(data, i))
			return (1);
		i++;
	}
	return (0);
}

void	supervisor_loop(t_data *data)
{
	while (1)
	{
		pthread_mutex_lock(&data->sim_mutex);
		if (data->threads_done >= data->nb_coders)
			return (pthread_mutex_unlock(&data->sim_mutex), (void)0);
		if (check_all_burnouts(data))
			return ;
		pthread_mutex_unlock(&data->sim_mutex);
		usleep(1000);
	}
}

int	start_simulation(t_data *data)
{
	int	i;

	if (!launch_threads(data))
		return (0);
	supervisor_loop(data);
	pthread_mutex_lock(&data->sim_mutex);
	data->simulation_over = 1;
	pthread_mutex_unlock(&data->sim_mutex);
	i = 0;
	while (i < data->nb_coders)
	{
		pthread_mutex_lock(&data->dongles[i].mutex);
		pthread_cond_broadcast(&data->dongles[i].cond);
		pthread_mutex_unlock(&data->dongles[i++].mutex);
	}
	i = 0;
	while (i < data->nb_coders)
		pthread_join(data->coders[i++].thread_id, NULL);
	return (1);
}

void	heap_remove(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->nodes[i].coder_id == coder_id)
		{
			heap->nodes[i] = heap->nodes[heap->size - 1];
			heap->size--;
			if (i < heap->size)
			{
				sift_up(heap, i);
				sift_down(heap, i);
			}
			return ;
		}
		i++;
	}
}
