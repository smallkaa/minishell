/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 21:20:30 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/24 08:09:00 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <termios.h>

void setup_signal_handlers(void);
void handle_sigint(int sig);
void handle_sigquit(int sig);
extern volatile sig_atomic_t	g_signal_flag;
