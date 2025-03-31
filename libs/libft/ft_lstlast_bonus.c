/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/22 15:09:33 by pvershin          #+#    #+#             */
/*   Updated: 2024/04/22 15:09:56 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*tmp;
	int		len;
	int		i;

	i = 0;
	tmp = lst;
	len = ft_lstsize(lst) - 1;
	while (i < len)
	{
		tmp = tmp->next;
		i++;
	}
	return (tmp);
}
