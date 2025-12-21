/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marapovi <marapovi@student.42vienna.c      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 15:45:46 by marapovi          #+#    #+#             */
/*   Updated: 2025/12/21 15:46:30 by marapovi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef PARSER_H
# define PARSER_H

t_token *tokenize(const char *line, t_shell *shell);
int      check_syntax(t_token *tokens);
void     expand_tokens(t_token *tokens, t_shell *shell);
t_cmd   *parse_tokens(t_token *tokens);

void     free_tokens(t_token *tokens);
void     free_cmds(t_cmd *cmds);

#endif //!PARSER_H
