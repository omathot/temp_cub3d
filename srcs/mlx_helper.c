/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mlx_helper.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: oscarmathot <oscarmathot@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 15:25:31 by oscarmathot       #+#    #+#             */
/*   Updated: 2024/03/18 15:48:36 by oscarmathot      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../cub3d.h"

char	*get_data_line(t_param_mlx *param, char x);
void	initialize_wall_info(t_wall_info *wall);
char	determine_face(t_param_mlx *param, int i);
void	wall_texture(t_param_mlx *param, int screen_x,
			t_wall_info wall, double wall_height);

void	save_file_data(t_param_mlx *param)
{
	param->map.wall_N = mlx_load_png(get_data_line(param, 'N'));
	param->map.wall_S = mlx_load_png(get_data_line(param, 'S'));
	param->map.wall_E = mlx_load_png(get_data_line(param, 'E'));
	param->map.wall_W = mlx_load_png(get_data_line(param, 'W'));
}

void	print_all_walls(t_param_mlx *param_real)
{
	int			i;
	t_wall_info	wall;

	initialize_wall_info(&wall);
	mlx_delete_image(param_real->mlx, param_real->image_to_draw_pixel);
	param_real->image_to_draw_pixel = mlx_new_image(
			param_real->mlx, param_real->x_resolution,
			param_real->y_resolution);
	i = 0;
	while (param_real->current_visible_walls[i])
	{
		wall.collision = *param_real->current_visible_walls[i];
		wall.wall_face = determine_face(param_real, i);
		wall_texture(param_real, i, wall, 400);
		i++;
	}
	mlx_image_to_window(param_real->mlx, param_real->image_to_draw_pixel, 0, 0);
}

void	print_point(char *start_message, t_point point)
{
	printf("%s (%f, %f)\n", start_message, point.x, point.y);
}

double	find_distance(t_point a, t_point b)
{
	return (sqrt(pow(a.y - b.y, 2) + pow(a.x - b.x, 2)));
}

double	radiant_to_dregre_angle(double angle)
{
	return (angle * (180.0 / M_PI));
}
