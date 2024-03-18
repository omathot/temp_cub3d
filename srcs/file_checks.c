/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file_checks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscarmathot <oscarmathot@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/04 18:51:58 by oscarmathot       #+#    #+#             */
/*   Updated: 2024/03/18 15:17:37 by oscarmathot      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

void	init_file_reqs(t_file_reqs **reqs);
int		check_map(char **map, int pos);
char	**make_filedata(char **content, int start);
char	**make_board(char **content, int start);
void	set_cnf_reqs(char ref, t_file_reqs **reqs);
int		check_texture(char *str, int *i, t_file_reqs **check);
int		other_checks(char *str, t_file_loc **locs, int *i, t_file_reqs **check);
int		last_checks(char *str, t_file_loc **locs, int *i, t_file_reqs **check);
int		check_nm(int *i, char *str, char **sprite_loc);

int	check_cnf(char *str, int *i, t_file_reqs **reqs, char ref)
{
	char	**nbrs;
	int		j;

	(*i) += 1;
	while (str[(*i)])
	{
		j = 0;
		while (ft_isspace(str[(*i)]))
			(*i)++;
		if (no_numbers(str, i) == 1)
			return (1);
		nbrs = ft_split(&str[(*i)], ',');
		while (nbrs[j])
		{
			if (ft_atoi(nbrs[j]) > 255 || ft_atoi(nbrs[j]) < 0)
			{
				free_double_char(nbrs);
				return (1);
			}
			j++;
		}
		(*i)++;
	}
	set_cnf_reqs(ref, reqs);
	return (0);
}

int	check_line(char *str, t_file_reqs **reqs)
{
	int	i;

	i = 0;
	while (ft_isspace(str[i]))
		i++;
	if (str[i] == 'N' || str[i] == 'S' || str[i] == 'E' || str[i] == 'W')
	{
		if (check_texture(str, &i, reqs) == 1)
			return (1);
	}
	else if (str[i] == 'C' || str[i] == 'F')
	{
		if (check_cnf(str, &i, reqs, str[i]) == 1)
			return (1);
	}
	else if (ft_isdigit(str[i]))
		return (2);
	else
		return (1);
	return (0);
}

int	check_reqs(t_file_reqs *reqs)
{
	if (reqs->c != 1 || reqs->ea != 1 || reqs->f != 1 || reqs->no != 1
		|| reqs->so != 1 || reqs->we != 1)
		return (1);
	else if (reqs->c == 1 || reqs->ea == 1 || reqs->f == 1 || reqs->no == 1
		|| reqs->so == 1 || reqs->we == 1)
		return (2);
	return (0);
}

int	final_format_check(t_file_reqs *reqs, t_map *map, int i)
{
	if (check_reqs(reqs) == 1)
		return (1);
	if (check_map((*map).content, i) == 1)
		return (1);
	return (0);
}

int	check_format(t_map *map)
{
	int			i;
	int			res;
	t_file_reqs	*reqs;

	reqs = (t_file_reqs *)malloc(sizeof(t_file_reqs));
	i = 0;
	init_file_reqs(&reqs);
	while ((*map).content[i])
	{
		res = check_line((*map).content[i], &reqs);
		if (res == 1)
		{
			free(reqs);
			return (1);
		}
		else if (res == 2)
		{
			(*map).board = make_board((*map).content, i);
			(*map).file_data = make_filedata((*map).content, i);
			break ;
		}
		i++;
	}
	final_format_check(reqs, map, i);
	return (0);
}
