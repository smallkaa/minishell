/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pvershin <pvershin@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 21:20:30 by pvershin          #+#    #+#             */
/*   Updated: 2025/04/25 13:40:45 by pvershin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SIGNALS_H
# define SIGNALS_H
# include <termios.h>

void							setup_signal_handlers(void);
void							handle_sigint(int sig);
void							handle_sigquit(int sig);
void							disable_echoctl(void);
extern volatile sig_atomic_t	g_signal_flag;

#endif