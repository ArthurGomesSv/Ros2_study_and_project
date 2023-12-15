<h1> Introdução ao ROS2 e projeto de projetos 2 </h1>
<img alt="Static Badge" src="https://img.shields.io/badge/license-MIT-green"> <img alt="Static Badge" src="https://img.shields.io/badge/release_date-december-blue">
Arquivos desenvolvidos durante os estudos de ROS2 e para o controle de um levitador de bolas criado durante a matéria de Projetos 2 do curso de Engenharia Mecatrônica da USP. Esse projeto foi desenvolvido utilizando ROS2 humble no Ubuntu 22.04, RealSense D435 e ESP32 com micro-ROS.

> [!NOTE]
> Para a execução desse projeto é necessário realizar a instalação do [SDK da RealSense D435](https://dev.intelrealsense.com/docs/compiling-librealsense-for-linux-ubuntu-guide) e o pacote [ROS Wrapper for depth camera](https://github.com/IntelRealSense/realsense-ros) dentro do ROS2 workspace. Além disso, é necessário instalar e configurar um workspace para o [micro-ROS](https://github.com/micro-ROS/).
<h2> Lançar projeto via launch file </h2> 

Dentro do ROS2 workspace:
```
ros2 launch my_robot_bringup ball_float_app.launch.py
```

<h2> Lançar micro-ROS agent </h2> 
O arquivo main.c disponível em /src/microROS_interface/src deve estar embarcado na ESP32 e o pacote microROS_interface ter sido compilado.

<h3> Source no workspace </h3>
Dentro do micro-ROS workspace:

```
source install/setup.bash
```

<h3> Launch agent </h3>
Dentro do micro-ROS workspace:

```
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/'YOUR_USB_PORT'
```

<h2> Alteração dos parâmetros PID </h2> 
Os parâmetros do PID podem ser alterados dentro do arquivo launch my_robot_bringup ball_float_app.launch.py.

```python
control_node = Node(
        package="control_pkg",
        executable="control",
        parameters=[
            {"KP":4.0},
            {"KD":2.35},
            {"KI":0.0}
        ]
    )
```


<h2> Alteração da referência para o controle </h2> 
Para alterar a referência do controle deve-se enviar um valor inteiro no tópico /communication/inter_graph_ball_height_ref.


<h3> Alteração de forma manual: </h3>

```
ros2 topic pub -1 /communication/inter_graph_ball_height_ref std_msgs/msg/Int32 "{data: 'INT_VALUE'}"
```

