import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_dir = get_package_share_directory('mobile_robot') # Change if your package name is different

    # 1. Robot State Publisher (Loads your physical Xacro dimensions)
    rsp_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(pkg_dir, 'launch', 'rsp.launch.py')),
        launch_arguments={'use_sim_time': 'false'}.items() # Critical: Must be FALSE for the real world!
    )

    # 2. Master ESP32 micro-ROS Agent (Handles Throttle, Brakes, Lift, Steering)
    master_agent = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='master_agent',
        arguments=['serial', '--dev', '/dev/ttyUSB_MASTER'] # PLACEHOLDER: Update after USB Roulette
    )

    # 3. Sensor ESP32 micro-ROS Agent (Handles Echo, Gyro, Warnings)
    sensor_agent = Node(
        package='micro_ros_agent',
        executable='micro_ros_agent',
        name='sensor_agent',
        arguments=['serial', '--dev', '/dev/ttyUSB_SENSOR'] # PLACEHOLDER: Update after USB Roulette
    )

    # 4. LiDAR Node
    lidar_node = Node(
        package='sllidar_ros2',
        executable='sllidar_node',
        name='sllidar_node',
        parameters=[{'serial_port': '/dev/ttyUSB_LIDAR', # PLACEHOLDER: Update after USB Roulette
                     'frame_id': 'lidar_link',
                     'angle_compensate': True}]
    )

    # 5. LiDAR Odometry (The mathematical magic that replaces wheel encoders)
    rf2o_node = Node(
        package='rf2o_laser_odometry',
        executable='rf2o_laser_odometry_node',
        name='rf2o_laser_odometry',
        output='screen',
        parameters=[{
            'laser_scan_topic' : '/scan',
            'odom_topic' : '/odom',
            'publish_tf' : True,
            'base_frame_id' : 'base_link', # Ensure this matches your robot.xacro base frame
            'odom_frame_id' : 'odom',
            'freq' : 20.0
        }]
    )

    # 6. GPS Node
    gps_node = Node(
        package='nmea_navsat_driver',
        executable='nmea_serial_driver',
        name='nmea_serial_driver',
        parameters=[{
            'port': '/dev/ttyUSB_GPS', # PLACEHOLDER: Update after USB Roulette
            'baud': 9600,
            'frame_id': 'gps_link'
        }]
    )

    return LaunchDescription([
        rsp_launch,
        master_agent,
        sensor_agent,
        lidar_node,
        rf2o_node,
        gps_node
    ])
