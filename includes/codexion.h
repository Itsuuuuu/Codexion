/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 12:11:57 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/09 22:04:44 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

typedef enum e_sched
{
	FIFO = 1,
	EDF = 2
}	t_sched;

typedef struct s_heap_node
{
	int			coder_id;
	long long	priority;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	*nodes;
	int			capacity;
	int			size;
}	t_heap;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				is_available;
	long long		cooldown_end;
	t_heap			heap;
}	t_dongle;

typedef struct s_data	t_data;

typedef struct s_coder
{
	int			id;
	pthread_t	thread_id;
	long long	last_compile_start;
	int			compiles_count;
	t_dongle	*left_dongle;
	t_dongle	*right_dongle;
	t_data		*data;
	int			is_compiling;
}	t_coder;

struct s_data
{
	int				nb_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				compiles_required;
	long long		dongle_cooldown;
	t_sched			scheduler_type;
	long long		start_time;
	int				simulation_over;
	int				threads_done;
	pthread_mutex_t	sim_mutex;
	pthread_mutex_t	print_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
};

//utils.c
long long	get_time_ms(void);
int			check_sim_over(t_data *data);
void		ft_usleep(long long time_in_ms, t_data *data);
void		print_status(t_coder *coder, char *status);
void		unlock_dongle(t_dongle *dongle, t_data *data);

//heap.c
int			heap_push(t_heap *heap, int coder_id, long long priority);
void		heap_pop(t_heap *heap);

//init.c
int			init_simulation(t_data *data);
void		supervisor_loop(t_data *data);
int			start_simulation(t_data *data);

//simulation.c
int			check_burnout(t_data *data, int i);
void		*coder_routine(void *arg);

/* dongles.c*/
void		acquire_dongles(t_coder *coder);
void		release_dongles(t_coder *coder);

//main.c
int			parse_arguments(t_data *data, int ac, char **av);
void		clean_simulation(t_data *data);

void		heap_remove(t_heap *heap, int coder_id);
void		sift_up(t_heap *heap, int index);
void		sift_down(t_heap *heap, int index);
#endif