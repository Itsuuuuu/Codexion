/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:27:15 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/07 23:33:06 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static void	assign_arguments(t_data *data, char **av)
{
	data->nb_coders = atoi(av[1]);
	data->time_to_burnout = atoi(av[2]);
	data->time_to_compile = atoi(av[3]);
	data->time_to_debug = atoi(av[4]);
	data->time_to_refactor = atoi(av[5]);
	data->compiles_required = atoi(av[6]);
	data->dongle_cooldown = atoi(av[7]);
	data->scheduler_type = FIFO;
	if (strcmp(av[8], "edf") == 0)
		data->scheduler_type = EDF;
}

int	parse_arguments(t_data *data, int ac, char **av)
{
	int	i;

	if (ac != 9)
		return (write(2, "Error: Wrong arguments count\n", 29), 0);
	i = 1;
	while (i <= 7)
	{
		if (!is_valid_number(av[i]))
			return (write(2, "Error: Invalid argument value\n", 30), 0);
		i++;
	}
	if (strcmp(av[8], "fifo") != 0 && strcmp(av[8], "edf") != 0)
		return (write(2, "Error: Invalid scheduler type\n", 30), 0);
	assign_arguments(data, av);
	return (1);
}

void	clean_simulation(t_data *data)
{
	int	i;

	if (data->dongles)
	{
		i = 0;
		while (i < data->nb_coders)
		{
			pthread_mutex_destroy(&data->dongles[i].mutex);
			pthread_cond_destroy(&data->dongles[i].cond);
			if (data->dongles[i].heap.nodes)
				free(data->dongles[i].heap.nodes);
			i++;
		}
		free(data->dongles);
	}
	if (data->coders)
		free(data->coders);
	pthread_mutex_destroy(&data->sim_mutex);
	pthread_mutex_destroy(&data->print_mutex);
}

int	main(int ac, char **av)
{
	t_data	data;

	memset(&data, 0, sizeof(t_data));
	if (!parse_arguments(&data, ac, av))
		return (1);
	if (!init_simulation(&data))
	{
		clean_simulation(&data);
		return (1);
	}
	if (!start_simulation(&data))
	{
		clean_simulation(&data);
		return (1);
	}
	clean_simulation(&data);
	return (0);
}
