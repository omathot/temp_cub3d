/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omathot <omathot@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/06 14:02:17 by oscarmathot       #+#    #+#             */
/*   Updated: 2024/02/08 15:09:08 by omathot          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

bool	is_out_of_bound(int x, int y, char **board)
{
	int i;

	i = 0;
	while (board[i])
	{
		i++;
	}
	if (y < i)
		return (false);
	int j;
	i = 0;
	while (board[i])
	{
		j = 0;
		while (board[i][j])
		{
			j++;
		}
		if (i == y)
			if (j <= x)
				return (false);
		i++;
	}
	return (true);
}

double calculate_slope(t_point p1, t_point p2) {
	return (p2.y - p1.y) / (p2.x - p1.x);
}

bool	*find_wall(char **board, t_point cur_coord, t_player player, t_point *good_coord, double player_offset_x, double player_offset_y)
{
	static bool	find;
	find = false;
	cur_coord.y -= player_offset_y; 
	cur_coord.x -= player_offset_x;
	if ((int)round(-cur_coord.y + player.pos.y) < 0 || (int)round(cur_coord.x + player.pos.x) < 0)
	{
		puts("found wall at neg values");
		pause();
		return (NULL);
	}
	if (is_out_of_bound((int)round(cur_coord.x + player.pos.x), (int)round(-cur_coord.y + player.pos.y), board))
	{
		puts("looking beyound map");
		print_point("cur coord", cur_coord);
		print_point("player pos", player.pos);
		return (NULL);
	}
	if (board[(int)round(-cur_coord.y + player.pos.y)][(int)round(cur_coord.x + player.pos.x)] == '\0')
	{
		puts("fazed out bro");
		return (NULL);
	}
	if (board[(int)round(-cur_coord.y + player.pos.y)][(int)round(cur_coord.x + player.pos.x)] == '1')
	{
		*good_coord = mk_point((cur_coord.x + player.pos.x), (-cur_coord.y + player.pos.y)); 
		find = true;
		return (&find);
	}
	return (&find);
}

bool	double_is_zero_modular_tolerence(double number, double tolerence)
{
	if (fabs(number) < tolerence)
		return (true);
	return (false);
}

bool	is_double_pretty_much_zero(double number)
{
	const double	tolerence = 0.000001;
	if (fabs(number) < tolerence)
		return (true);
	return (false);
}

double	round_next_clossest_half(double number)
{
	return (ceil((number) * 2) / 2);
}

double	round_clossest_half(double number)
{
	return (floor((number) * 2) / 2);
}

t_point	get_wall_cell_for_wall(double current_x, double current_y, bool x_neg, bool y_neg, double slope, double change_wall_size_look)
{
	t_point	wall_cor;
	if (fabs(slope) < 1)
	{
		if (!y_neg)
			wall_cor = mk_point(current_x, current_y - change_wall_size_look);
		else 
			wall_cor = mk_point(current_x, current_y + change_wall_size_look);
	}
	else
	{
		if (!x_neg)
			wall_cor = mk_point(current_x - change_wall_size_look, current_y);
		else
			wall_cor = mk_point(current_x + change_wall_size_look, current_y);
	}
	return (wall_cor);
}

t_point	rc_get_next_y_incrementation_wall(double *current_x, double *current_y, double slope, double change_wall_size_look, bool x_neg, bool y_neg)
{
	double	current_x_round;
	t_point	cur_coord;


	if (!y_neg)
		(*current_y) = round_clossest_half((*current_y) + change_wall_size_look);
	else
		(*current_y) = -round_clossest_half(-(*current_y) + change_wall_size_look);
	if (fabs(*current_x) > 0.000)
	{
		if (!x_neg)
		{
			current_x_round = round_next_clossest_half((*current_x));
		}
		else
		{
			current_x_round = -round_next_clossest_half(-(*current_x));
		}
	}
	else
		current_x_round = 0;
	double past_x = (*current_x);
	bool	is_x_closser_to_previous_ray = false;
	(*current_x) = (*current_y) / slope;
	if (!x_neg)
	{
		if (current_x_round <= (*current_x) && !is_double_pretty_much_zero(current_x_round - past_x))
			is_x_closser_to_previous_ray = true;
	}
	else
	{
		if (current_x_round >= (*current_x) && !is_double_pretty_much_zero(current_x_round - past_x))
			is_x_closser_to_previous_ray = true;
	}
	if (is_x_closser_to_previous_ray)
	{
		(*current_x) = current_x_round;
		(*current_y) = (*current_x) * slope;
		if (!y_neg)
			cur_coord = mk_point((*current_x), (*current_y) - change_wall_size_look);
		else
			cur_coord = mk_point((*current_x), (*current_y) + change_wall_size_look);
	}
	else
		if (!x_neg)
		{
			cur_coord = mk_point((*current_x) - change_wall_size_look, (*current_y));
		}
		else
		{
			cur_coord = mk_point((*current_x) + change_wall_size_look, (*current_y));
		}
	// cur_coord = get_wall_cell_for_wall((*current_x), (*current_y), x_neg, y_neg, slope, change_wall_size_look);
	return (cur_coord);
}



t_point	rc_get_next_x_incrementation_wall(double *current_x, double *current_y, double slope, double change_wall_size_look, bool x_neg, bool y_neg)
{
	t_point	cur_coord;
	double	current_y_round;
	if (!x_neg)
		(*current_x) = round_clossest_half((*current_x) + change_wall_size_look);
	else
		(*current_x) = -round_clossest_half(-(*current_x) + change_wall_size_look);
	if (fabs(*current_y) > 0.000)
	{
		if (!y_neg)
		{
			current_y_round = round_next_clossest_half((*current_y));
		}
		else
		{
			current_y_round = -round_next_clossest_half(-(*current_y));
		}
	}
	else
		current_y_round = 0;
	double past_y = (*current_y);

	bool	is_y_closser_to_wall = false;
	(*current_y) = (*current_x) * slope;
	if (!y_neg)
	{
		if (current_y_round <= (*current_y) && current_y_round != 0 && !is_double_pretty_much_zero(current_y_round - past_y))
			is_y_closser_to_wall = true;
	}
	else
	{
		if (current_y_round >= (*current_y) && current_y_round != 0 && !is_double_pretty_much_zero(current_y_round - past_y))
			is_y_closser_to_wall = true;
	}
	if (is_y_closser_to_wall)
	{
		(*current_y) = current_y_round;
		(*current_x) = (*current_y) / slope;
		if (!x_neg)
			cur_coord = mk_point((*current_x) - change_wall_size_look, (*current_y));
		else
			cur_coord = mk_point((*current_x) + change_wall_size_look, (*current_y));
	}
	else
	{
		if (!y_neg)
		{
			cur_coord = mk_point((*current_x), (*current_y) - change_wall_size_look);
		}
		else
		{
			cur_coord = mk_point((*current_x), (*current_y) + change_wall_size_look);
		}
	}
	// cur_coord = get_wall_cell_for_wall((*current_x), (*current_y), x_neg, y_neg, slope, change_wall_size_look);
	return (cur_coord);
}

t_point	*convert_good_coordinates_wall(t_point *good_coord, t_player player, double cur_x, double cur_y, double player_offset_x, double player_offset_y)
{
	// print_point("currnex and y are", mk_point(cur_x, cur_y));
	(*good_coord) = mk_point((cur_x + player.pos.x - player_offset_x), (-cur_y + player.pos.y - player_offset_y));
	// (*good_coord) = mk_point((cur_x), (-cur_y));
	return (good_coord);
}

t_point *ray_casting_to_find_wall(char **board, t_player player, double angle)
{
	int lenght_check = 10;
	double  randiant_angle_looking = (angle) * (M_PI / 180);


	t_vector    direction_ray;
	direction_ray.magnitude = lenght_check;
	direction_ray.angle = angle;
	t_point furthest_point = mk_point(lenght_check * sin(randiant_angle_looking), lenght_check * cos(randiant_angle_looking));
	double current_x = 0;
	double current_y = 0;
	// double current_x_round = 0;
	// double current_y_round = 0;
	double	player_offset_x = -(round((player.pos.x)) - player.pos.x);
	double	player_offset_y = (round((player.pos.y)) - player.pos.y);
	// double	player_offset_x = -(player.pos.x - round((player.pos.x)));
	// double	player_offset_y = (player.pos.y - round((player.pos.y)));

	// if (is_double_pretty_much_zero(player_offset_x))
		// player_offset_x = 0;
	// if (is_double_pretty_much_zero(player_offset_y))
		// player_offset_y = 0;
	// player_offset_x = 0;
	// player_offset_y = 0;
	if (furthest_point.x < -0.00001)
		player_offset_x += 0.0001;
	// if (furthest_point.y < -0.00001)
	// 	player_offset_y += 0.001;
	// if (furthest_point.y < -0.00001)
	// 	player_offset_y += 0.001;
	// player_offset_x = (player.pos.x - round(player.pos.x)) - 0.5;
    // player_offset_y = (player.pos.y - round(player.pos.y)) - 0.5;
	// printf("player x off set is %f, y is %f\n", player_offset_x, player_offset_y);
	t_point cur_coord;
	// double	delta_x = sin((direction_ray.angle) * (M_PI / 180)) / 2;
	// double	delta_y = cos((direction_ray.angle) * (M_PI / 180)) / 2;
	double	change_wall_size_look = 0.5;
	t_point *good_coord = (t_point *)malloc(sizeof(t_point));
	bool *good;
	double	slope = calculate_slope(mk_point(0,0), furthest_point);
	t_orientation	wall_face;
	cur_coord = mk_point(0, 0);
	current_y = 0;
	current_x = 0;
	// current_y = (player.pos.x - round((player.pos.x)));
	// current_x = (player.pos.y - round((player.pos.y)));
	bool	first = true;
		while (furthest_point.x != floor(cur_coord.x) || furthest_point.y != floor(cur_coord.y))
		{
			// printf("curx = %f, cury = %f\n", current_x, current_y);
			if ((furthest_point.x > 0.000001 && furthest_point.y > 0.000001) || is_double_pretty_much_zero(furthest_point.x))
			{
				if (is_double_pretty_much_zero(furthest_point.x))
				{
					if (furthest_point.y < -0.000001)
					{
						current_y = round_clossest_half(current_y - change_wall_size_look);
						cur_coord = mk_point(current_x, current_y + change_wall_size_look);
						// pause();
					}
					else
					{
						current_y = round_clossest_half(current_y + change_wall_size_look);
						cur_coord = mk_point(current_x, current_y - change_wall_size_look);
					}
				}
				// this very weard
				else if (round_clossest_half(current_x + change_wall_size_look) / slope < round_clossest_half(current_y + change_wall_size_look))
					cur_coord = rc_get_next_y_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, false, false);
				else
					cur_coord = rc_get_next_x_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, false, false);
				good = find_wall(board, cur_coord, player, good_coord, player_offset_x, player_offset_y);
				if (good == NULL)
					puts("raycast out of bound 1");
				else if (*good)
					return (convert_good_coordinates_wall(good_coord, player, current_x, current_y, player_offset_x, player_offset_y));
			}
			else if ((furthest_point.x < -0.000001 && furthest_point.y > 0.000001) || is_double_pretty_much_zero(furthest_point.y))
			{
				if (is_double_pretty_much_zero(furthest_point.y))
				{
					if (furthest_point.x < -0.000001)
					{
						current_x = round_clossest_half(current_x - change_wall_size_look);
						cur_coord = mk_point(current_x + change_wall_size_look, current_y);
					}
					else
					{
						current_x = round_clossest_half(current_x + change_wall_size_look);
						cur_coord = mk_point(current_x - change_wall_size_look, current_y);
					}
				}
				else if (round_clossest_half(current_x - change_wall_size_look) / slope < round_clossest_half(current_y + change_wall_size_look))
					cur_coord = rc_get_next_y_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, true, false);
				else
					cur_coord = rc_get_next_x_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, true, false);
				good = find_wall(board, cur_coord, player, good_coord, player_offset_x, player_offset_y);
				if (good == NULL)
					puts("raycast out of bound 1");
				else if (*good)
					return (convert_good_coordinates_wall(good_coord, player, current_x, current_y, player_offset_x, player_offset_y));
			}
			else if (furthest_point.x > +0.000001 && furthest_point.y < -0.000001)
			{
				if (round_clossest_half(current_x + change_wall_size_look) / slope > round_clossest_half(current_y - change_wall_size_look))
					cur_coord = rc_get_next_y_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, false, true);
				else
					cur_coord = rc_get_next_x_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, false, true);
				good = find_wall(board, cur_coord, player, good_coord, player_offset_x, player_offset_y);
				if (good == NULL)
					puts("raycast out of bound 1");
				else if (*good)
					return (convert_good_coordinates_wall(good_coord, player, current_x, current_y, player_offset_x, player_offset_y));
			}
			else if (furthest_point.x < -0.000001 && furthest_point.y < -0.000001)
			{
				if (round_clossest_half(current_x - change_wall_size_look) / slope > round_clossest_half(current_y - change_wall_size_look))
					cur_coord = rc_get_next_y_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, true, true);
				else
					cur_coord = rc_get_next_x_incrementation_wall(&current_x, &current_y, slope, change_wall_size_look, true, true);
				good = find_wall(board, cur_coord, player, good_coord, player_offset_x, player_offset_y);
				if (good == NULL)
					puts("raycast out of bound 1");
				else if (*good)
					return (convert_good_coordinates_wall(good_coord, player, current_x, current_y, player_offset_x, player_offset_y));
			}
		}
	puts("didn't found wall");
	puts("");
	return (NULL);
}

t_point **view_walls(t_map map, int x_resolution)
{
	t_point **all_walls;
	double angle_drift = map.player.angle_view / x_resolution;
	int     current_wall_index = 0;
	double  current_angle = map.player.angle - (map.player.angle_view  / 2);
	double  final_angle = map.player.angle + (map.player.angle_view  / 2);
	double  number_of_found_walls = ceil(map.player.angle_view  / angle_drift);

	all_walls = malloc(sizeof(t_point *) * (number_of_found_walls + 3));
	while (final_angle > current_angle)
	{
		t_point *current_wall;
		current_wall =  ray_casting_to_find_wall(map.board, map.player, current_angle);
		// print_point("current map is", (*current_wall));
		if (current_wall != NULL)
		{
			if (current_wall_index == 0)
			{
				all_walls[current_wall_index] = current_wall;
				current_wall_index++;
			}
			else if ((*current_wall).x == (*all_walls[current_wall_index - 1]).x && (*current_wall).y == (*all_walls[current_wall_index - 1]).y)
			{
				// puts("dup wall");
			}
			else
			{
				all_walls[current_wall_index] = current_wall;
				current_wall_index++;
			}
		}
		current_angle = current_angle + angle_drift;
	}
	all_walls[current_wall_index] = NULL;
	// printf("curent walls are %i\n", current_wall_index);
	return (all_walls);
}