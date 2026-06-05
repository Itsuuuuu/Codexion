/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/02 12:17:33 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/05 15:41:13 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_burnout(t_data *data, int i)
{
	if (data->compiles_required > 0
		&& data->coders[i].compiles_count >= data->compiles_required)
		return (0);
	if (get_time_ms() - data->coders[i].last_compile_start
		> data->time_to_burnout)
	{
		data->simulation_over = 1;
		pthread_mutex_unlock(&data->sim_mutex);
		print_status(&data->coders[i], "burned out");
		return (1);
	}
	return (0);
}

static int	execute_compile(t_coder *coder, t_data *data)
{
	if (check_sim_over(data))
	{
		release_dongles(coder);
		return (0);
	}
	pthread_mutex_lock(&data->sim_mutex);
	coder->compiles_count++;
	pthread_mutex_unlock(&data->sim_mutex);
	print_status(coder, "is compiling");
	ft_usleep(data->time_to_compile, data);
	if (coder->data->scheduler_type == EDF)
		usleep(500);
	release_dongles(coder);
	return (1);
}

static int	wait_if_done(t_coder *coder, t_data *data)
{
	if (data->compiles_required > 0
		&& coder->compiles_count >= data->compiles_required)
	{
		while (!check_sim_over(data))
			usleep(1000);
		return (1);
	}
	return (0);
}

static int	coder_cycle(t_coder *coder, t_data *data)
{
	if (check_sim_over(data))
		return (0);
	print_status(coder, "is refactoring");
	ft_usleep(data->time_to_refactor, data);
	if (wait_if_done(coder, data))
		return (0);
	pthread_mutex_lock(&data->sim_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&data->sim_mutex);
	acquire_dongles(coder);
	if (check_sim_over(coder->data))
		return (0);
	if (!execute_compile(coder, data))
		return (0);
	if (data->scheduler_type == EDF)
		usleep(500);
	print_status(coder, "is debugging");
	ft_usleep(data->time_to_debug, data);
	return (1);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_data	*data;

	coder = (t_coder *)arg;
	data = coder->data;
	if (coder->id % 2 == 0)
		ft_usleep(data->time_to_compile / 2, data);
	if (wait_if_done(coder, data))
		return (NULL);
	while (1)
	{
		if (!coder_cycle(coder, data))
			return (NULL);
	}
	return (NULL);
}
