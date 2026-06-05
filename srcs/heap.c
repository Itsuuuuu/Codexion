/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: guifouqu <guifouqu@student.42lehavre.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 16:18:55 by guifouqu          #+#    #+#             */
/*   Updated: 2026/06/05 15:27:16 by guifouqu         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_nodes(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static void	sift_up(t_heap *heap, int index)
{
	int	parent;

	parent = (index - 1) / 2;
	while (index > 0 && (heap->nodes[index].priority < \
heap->nodes[parent].priority || (heap->nodes[index].priority == \
heap->nodes[parent].priority && heap->nodes[index].coder_id > \
heap->nodes[parent].coder_id)))
	{
		swap_nodes(&heap->nodes[index], &heap->nodes[parent]);
		index = parent;
		parent = (index - 1) / 2;
	}
}

static void	sift_down(t_heap *heap, int index)
{
	int	min;
	int	left;
	int	right;

	min = index;
	left = 2 * index + 1;
	right = 2 * index + 2;
	if (left < heap->size && (heap->nodes[left].priority < \
heap->nodes[min].priority || (heap->nodes[left].priority == \
heap->nodes[min].priority && heap->nodes[left].coder_id > \
heap->nodes[min].coder_id)))
		min = left;
	if (right < heap->size && (heap->nodes[right].priority < \
heap->nodes[min].priority || (heap->nodes[right].priority == \
heap->nodes[min].priority && heap->nodes[right].coder_id > \
heap->nodes[min].coder_id)))
		min = right;
	if (min != index)
	{
		swap_nodes(&heap->nodes[index], &heap->nodes[min]);
		sift_down(heap, min);
	}
}

int	heap_push(t_heap *heap, int coder_id, long long priority)
{
	if (heap->size >= heap->capacity)
		return (0);
	heap->nodes[heap->size].coder_id = coder_id;
	heap->nodes[heap->size].priority = priority;
	sift_up(heap, heap->size);
	heap->size++;
	return (1);
}

void	heap_pop(t_heap *heap)
{
	if (heap->size <= 0)
		return ;
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];
	sift_down(heap, 0);
}
