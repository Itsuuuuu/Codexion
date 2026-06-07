/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 10:02:32 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/07 22:40:52 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_dongles(t_data *data)
{
	int		i;
	size_t	size;

	i = 0;
	size = sizeof(t_heap_node) * data->nb_coders;
	while (i < data->nb_coders)
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
		data->dongles[i].heap.nodes = malloc(size);
		if (!data->dongles[i].heap.nodes)
			return (0);
		i++;
	}
	return (1);
}

static void	init_coders(t_data *data)
{
	int			i;
	long long	start;

	i = 0;
	start = get_time_ms();
	while (i < data->nb_coders)
	{
		data->coders[i].id = i + 1;
		data->coders[i].last_compile_start = start;
		data->coders[i].compiles_count = 0;
		data->coders[i].data = data;
		data->coders[i].left_dongle = &data->dongles[i];
		data->coders[i].right_dongle = &data->dongles[(i + 1) % \
data->nb_coders];
		i++;
	}
}

int	init_simulation(t_data *data)
{
	data->coders = malloc(sizeof(t_coder) * data->nb_coders);
	data->dongles = malloc(sizeof(t_dongle) * data->nb_coders);
	if (!data->coders || !data->dongles)
		return (0);
	if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
		return (0);
	if (!init_dongles(data))
		return (0);
	init_coders(data);
	data->simulation_over = 0;
	data->threads_done = 0;
	data->start_time = get_time_ms();
	return (1);
}
