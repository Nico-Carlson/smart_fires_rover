import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

def generate_launch_description():
    # 1. Find the official Nav2 bringup package
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    # 2. Find your custom parameters file
    nav2_params_path = os.path.join(
        get_package_share_directory('mobile_robot'),
        'config',
        'nav2_params.yaml'
    )

    # 3. Call the official navigation script but inject your custom settings
    nav2_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_bringup_dir, 'launch', 'navigation_launch.py')
        ),
        launch_arguments={
            'use_sim_time': 'true',
            'params_file': nav2_params_path,
            'autostart': 'true'
        }.items()
    )

    return LaunchDescription([
        nav2_launch
    ])
