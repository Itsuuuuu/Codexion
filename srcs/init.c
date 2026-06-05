/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 10:02:32 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/05 15:27:30 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_data *data)
{
	int		i;
	size_t	heap_n;

	i = -1;
	heap_n = sizeof(t_heap_node) * data->nb_coders;
	while (++i < data->nb_coders)
	{
		data->dongles[i].id = i + 1;
		data->dongles[i].is_available = 1;
		data->dongles[i].cooldown_end = 0;
		if (pthread_mutex_init(&data->dongles[i].mutex, NULL) != 0)
			return (0);
		if (pthread_cond_init(&data->dongles[i].cond, NULL) != 0)
			return (0);
		data->dongles[i].heap.capacity = data->nb_coders;
		data->dongles[i].heap.size = 0;
		data->dongles[i].heap.nodes = malloc(heap_n);
		if (!data->dongles[i].heap.nodes)
			return (0);
	}
	return (1);
}

static void	init_coders(t_data *data)
{
	t_coder		*coder;
	int			i;
	long long	start_time;

	i = 0;
	start_time = get_time_ms();
	while (i < data->nb_coders)
	{
		coder = &data->coders[i];
		coder->id = i + 1;
		coder->last_compile_start = start_time;
		coder->compiles_count = 0;
		coder->data = data;
		coder->left_dongle = &data->dongles[i];
		coder->right_dongle = &data->dongles[(i + 1) % data->nb_coders];
		i++;
	}
}

int	init_simulation(t_data *data)
{
	data->coders = malloc(sizeof(t_coder) * data->nb_coders);
	data->dongles = malloc(sizeof(t_dongle) * data->nb_coders);
	if (data->coders == 0 || data->dongles == 0)
		return (0);
	if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (0);
	if (!init_dongles(data))
		return (0);
	init_coders(data);
	data->simulation_over = 0;
	data->start_time = get_time_ms();
	return (1);
}

void	supervisor_loop(t_data *data)
{
	int	i;
	int	done;

	while (1)
	{
		i = -1;
		done = 1;
		pthread_mutex_lock(&data->sim_mutex);
		while (++i < data->nb_coders)
		{
			if (check_burnout(data, i))
				return ;
			if (data->compiles_required > 0 && \
data->coders[i].compiles_count < data->compiles_required)
				done = 0;
		}
		if (data->compiles_required > 0 && done)
		{
			data->simulation_over = 1;
			pthread_mutex_unlock(&data->sim_mutex);
			return ;
		}
		pthread_mutex_unlock(&data->sim_mutex);
		usleep(1000);
	}
}

int	start_simulation(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->nb_coders)
	{
		pthread_mutex_lock(&data->sim_mutex);
		data->coders[i].last_compile_start = get_time_ms();
		pthread_mutex_unlock(&data->sim_mutex);
		if (pthread_create(&data->coders[i].thread_id, NULL, \
&coder_routine, &data->coders[i]) != 0)
			return (0);
	}
	supervisor_loop(data);
	i = -1;
	while (++i < data->nb_coders)
	{
		pthread_mutex_lock(&data->dongles[i].mutex);
		pthread_cond_broadcast(&data->dongles[i].cond);
		pthread_mutex_unlock(&data->dongles[i].mutex);
	}
	i = -1;
	while (++i < data->nb_coders)
		pthread_join(data->coders[i].thread_id, NULL);
	return (1);
}
