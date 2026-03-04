import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
import xacro

def generate_launch_description():

    # 1. Setup Paths
    robotXacroName = 'mobile_robot'
    namePackage = 'mobile_robot'
    modelFileRelativePath = 'model/robot.xacro'

    # absolute path
    pathModelFile = os.path.join(get_package_share_directory(namePackage), modelFileRelativePath)
    robotDescription = xacro.process_file(pathModelFile).toxml()

    gazebo_rosPackageLaunch = PythonLaunchDescriptionSource(
        os.path.join(get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')
    )

    # Path to the EKF config file
    ekf_config_file = os.path.join(get_package_share_directory(namePackage), 'config', 'ekf.yaml')

    # Find the path to your custom world
    worldFile = os.path.join(get_package_share_directory(namePackage), 'worlds', 'test_world.sdf')

    # Launch Gazebo WITH the custom world
    gazeboLaunch = IncludeLaunchDescription(
        gazebo_rosPackageLaunch,
        launch_arguments={'gz_args': f'-s-r {worldFile}', 'on_exit_shutdown': 'true'}.items()
    )

    # gazebo spawner node
    spawnModelNodeGazebo = Node(
        package='ros_gz_sim',
        executable='create',
        arguments=[
            '-name', robotXacroName,
            '-topic', 'robot_description'
        ],
        output='screen',
    )

    # robot state publisher node
    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': robotDescription,
            'use_sim_time': True
        }]
    )

    # control robot from ros2
    bridge_params = os.path.join(
        get_package_share_directory(namePackage),
        'parameters',
        'bridge_parameters.yaml'
    )

    start_gazebo_ros_bridge_cmd = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        arguments=[
            '--ros-args',
            '-p',
            f'config_file:={bridge_params}',
        ],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # create launch object
    launchDescriptionObject = LaunchDescription()

    # Path to your saved RViz config
    rviz_config_file = os.path.join(get_package_share_directory(namePackage), 'rviz', 'rover.rviz')

    # Tell RViz where Gazebo's internal sensor frame is located
    static_tf_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        arguments=[
            '--x', '0', '--y', '0', '--z', '0',
            '--yaw', '0', '--pitch', '0', '--roll', '0',
            '--frame-id', 'lidar_link',
            '--child-frame-id', 'mobile_robot/lidar_link/gpu_lidar'
        ],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # Robot Localization Node (odom EKF)
    robot_localization_node = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node',
        output='screen',
        parameters=[ekf_config_file, {'use_sim_time': True}],
        remappings=[('odometry/filtered', 'odometry/local')]
    )

    # Global Localization Node (Map EKF)
    global_ekf_node = Node(
        package='robot_localization',
        executable='ekf_node',
        name='ekf_filter_node_map',
        output='screen',
        parameters=[ekf_config_file, {'use_sim_time': True}],
        remappings=[('odometry/filtered', 'odometry/global')]
    )

    # GPS to XY Translator Node
    navsat_transform_node = Node(
        package='robot_localization',
        executable='navsat_transform_node',
        name='navsat_transform',
        output='screen',
        parameters=[ekf_config_file, {'use_sim_time': True}],
        remappings=[
            ('gps/fix', '/navsat'),
            ('imu', '/imu'),
            ('odometry/filtered', 'odometry/global')
        ]
    )

    # RViz node
    rvizNode = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        arguments=['-d', rviz_config_file],
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # add actions
    launchDescriptionObject.add_action(gazeboLaunch)
    launchDescriptionObject.add_action(spawnModelNodeGazebo)
    launchDescriptionObject.add_action(robot_state_publisher_node)
    launchDescriptionObject.add_action(start_gazebo_ros_bridge_cmd)
    launchDescriptionObject.add_action(static_tf_node)
    launchDescriptionObject.add_action(robot_localization_node)
    launchDescriptionObject.add_action(navsat_transform_node)
    launchDescriptionObject.add_action(global_ekf_node)
    launchDescriptionObject.add_action(rvizNode)

    return launchDescriptionObject
