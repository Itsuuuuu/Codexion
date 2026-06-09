/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:42:01 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/09 12:17:09 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000ULL) + (tv.tv_usec / 1000));
}

int	check_sim_over(t_data *data)
{
	int	over;

	pthread_mutex_lock(&data->sim_mutex);
	over = data->simulation_over;
	pthread_mutex_unlock(&data->sim_mutex);
	return (over);
}

void	ft_usleep(long long time_in_ms, t_data *data)
{
	long long	start_time;

	start_time = get_time_ms();
	while ((get_time_ms() - start_time) < time_in_ms)
	{
		if (check_sim_over(data))
			return ;
		usleep(500);
	}
}

void	print_status(t_coder *coder, char *status)
{
	long long	time;

	pthread_mutex_lock(&coder->data->print_mutex);
	if (!check_sim_over(coder->data) || !strcmp(status, "burned out"))
	{
		time = get_time_ms() - coder->data->start_time;
		printf("%lld Codeur %d %s\n", time, coder->id, status);
	}
	pthread_mutex_unlock(&coder->data->print_mutex);
}

void	unlock_dongle(t_dongle *dongle, t_data *data)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_available = 1;
	if (data->dongle_cooldown > 0)
		dongle->cooldown_end = get_time_ms() + data->dongle_cooldown;
	heap_pop(&dongle->heap);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}
