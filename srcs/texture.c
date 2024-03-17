/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscar <oscar@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/13 17:52:31 by oscarmathot       #+#    #+#             */
/*   Updated: 2024/03/17 16:27:47 by oscar            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

uint32_t	get_collor(int r, int g, int b, int a);

// Get the red channel
int	get_r(int rgba)
{
	return ((rgba >> 24) & 0xFF);
}

// Get the green channel
int	get_g(int rgba)
{
	return ((rgba >> 16) & 0xFF);
}

// Get the blue channel.
int	get_b(int rgba)
{
	return ((rgba >> 8) & 0xFF);
}

void	get_rgba(int i, mlx_image_t *to_place, t_rgba *rgb)
{
	int	rgba;

	rgba = (to_place->pixels[i * 4] << 24) | (to_place->pixels[i * 4 + 1] << 16)
		| (to_place->pixels[i * 4 + 2] << 8) | (to_place->pixels[i * 4 + 3]);
	(*rgb).r = get_r(rgba);
	(*rgb).g = get_g(rgba);
	(*rgb).b = get_b(rgba);
}

void	write_line(t_param_mlx *param, int i, int *j, char **to_return)
{
	int	k;

	k = 0;
	printf("test\n");
	while (param->map.file_data[i][(*j)])
	{
		(*to_return)[k] = param->map.file_data[i][(*j)];
		(*j)++;
		k++;
	}
	(*to_return)[k] = '\0';
	printf("data = (%s)", (*to_return));
}

char	*get_data_line(t_param_mlx *param, char x)
{
	char	*to_return;
	int		i;
	int		j;

	i = 0;
	to_return = (char *)malloc(sizeof(
				ft_strlen(param->map.file_data[i]) * 2 + 1));
	while (param->map.file_data[i])
	{
		j = 0;
		if (param->map.file_data[i][j] == x)
		{
			while (!(ft_isspace(param->map.file_data[i][j])))
				j++;
			j++;
			write_line(param, i, &j, &to_return);
		}
		i++;
	}
	return (to_return);
}

t_texture_data	normalize_collision_point(t_point collision_point, bool side)
{
	t_texture_data	data;

	if (!side)
	{
		data.wall_n = round(collision_point.x - 0.5);
		data.decimal = (collision_point.x - data.wall_n);
	}
	else
	{
		data.wall_n = round(collision_point.y - 0.5);
		data.decimal = (collision_point.y - data.wall_n);
	}
	return (data);
}

void	initialize_tex_variables(t_texture_vars *vars)
{
	vars->to_place = NULL;
	vars->screen_height = 720;
	vars->magnitude = 0;
	vars->texture_x = 0;
	vars->start_y = 0;
	vars->end_y = 0;
	vars->y_diff = 0;
	vars->texture_y = 0;
	vars->color = 0;
	vars->size = 0;
	vars->corrected_height = 0;
}

void	initialize_rgba(t_rgba *rgba)
{
	rgba->r = 0;
	rgba->b = 0;
	rgba->g = 0;
}

void	determine_texture(t_texture_vars *variables,
	t_wall_info wall, t_param_mlx *param)
{
	if (wall.wall_face == 'N')
		(*variables).to_place = mlx_texture_to_image(
				param->mlx, param->map.wall_N);
	else if (wall.wall_face == 'S')
		(*variables).to_place = mlx_texture_to_image(
				param->mlx, param->map.wall_S);
	else if (wall.wall_face == 'E')
		(*variables).to_place = mlx_texture_to_image(
				param->mlx, param->map.wall_E);
	else if (wall.wall_face == 'W')
		(*variables).to_place = mlx_texture_to_image(
				param->mlx, param->map.wall_W);
	(*variables).size = (variables->to_place->height
			* variables->to_place->width) - 1;
}

double	do_the_maths(t_param_mlx *param, int screen_x, double magnitude)
{
	double	angle_drift;
	double	current_angle;
	double	relative_angle;
	double	corrected_height;

	angle_drift = param->map.player.angle_view / param->x_resolution;
	current_angle = param->map.player.angle
		- (param->map.player.angle_view / 2);
	relative_angle = fabs(param->map.player.angle
			- (angle_drift * screen_x + current_angle));
	corrected_height = magnitude * cos(relative_angle * (M_PI / 180));
	corrected_height = magnitude;
	return (corrected_height);
}

void	clamp_xy(t_texture_vars *variables,
	double wall_height, t_param_mlx *param, t_wall_info wall)
{
	(*variables).magnitude = sqrt(pow((param->map.player.pos.x
					- wall.collision.x), 2) + pow(
				param->map.player.pos.y - wall.collision.y, 2));
	(*variables).normalized = normalize_collision_point(wall.collision,
			wall.wall_face == 'E' || wall.wall_face == 'W');
	(*variables).texture_x = (int)(variables->normalized.decimal
			* variables->to_place->width);
	(*variables).corrected_height = do_the_maths(param,
			param->x_resolution, variables->magnitude);
	(*variables).start_y = ((double)param->y_resolution / 2)
		- (wall_height / variables->corrected_height);
	(*variables).end_y = ((double)param->y_resolution / 2)
		+ (wall_height / variables->corrected_height);
	if (variables->texture_x < 0)
		(*variables).texture_x = 0;
	else if (variables->texture_x >= (int)variables->to_place->width)
		(*variables).texture_x = variables->to_place->width - 1;
	(*variables).y_diff = fabs(variables->start_y - variables->end_y) - 1;
}

//point.y = screen_hight. point.x = screen_x
void	place_wall_slice(t_texture_vars *variables,
	t_point point, t_param_mlx *param, t_rgba rgba)
{
	int	y;

	y = 0;
	while (y < variables->y_diff)
	{
		if (floor((*variables).start_y + y) >= variables->screen_height || floor((*variables).start_y + y) < 0)
		{
			y++;
			continue ;
		}
		(*variables).texture_y = (int)((y / (point.y
						/ variables->corrected_height) / 2)
				* variables->to_place->height);
		if (variables->texture_y < 0)
			(*variables).texture_y = 0;
		else if (variables->texture_y >= (int)variables->to_place->height)
			(*variables).texture_y = variables->to_place->height - 1;
		get_rgba((*variables).texture_y * (*variables).to_place->width
			+ (*variables).texture_x, (*variables).to_place, &rgba);
		(*variables).color = get_collor(rgba.r, rgba.g, rgba.b, 255);
		mlx_put_pixel(param->image_to_draw_pixel, floor(point.x),
			(floor((*variables).start_y) + y), (*variables).color);
		y++;
	}
}


void	wall_texture(t_param_mlx *param, int screen_x,
	t_wall_info wall, double wall_height)
{
	t_texture_vars	variables;
	t_rgba			rgba;

	initialize_tex_variables(&variables);
	
	initialize_rgba(&rgba);
	determine_texture(&variables, wall, param);
	clamp_xy(&variables, wall_height, param, wall);
	place_wall_slice(&variables, mk_point((double)screen_x,
			wall_height), param, rgba);
	mlx_delete_image(param->mlx, variables.to_place);
}
