#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/image.hpp"
#include "std_msgs/msg/header.hpp"
#include "std_msgs/msg/int32.hpp"
#include <chrono>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.hpp>
#include <opencv2/opencv.hpp>

//Topico da depth image /camera/camera/aligned_depth_to_color/image_raw
//Topico da intrinsic matrix /camera/camera/depth/camera_info

class ImageProcessingNode : public rclcpp::Node {
public:
  ImageProcessingNode() : Node("opencv_image_publisher"){
    //Create subscriber
    realsense_image_depth_subscriber_ = this->create_subscription<sensor_msgs::msg::Image>("/camera/camera/aligned_depth_to_color/image_raw",
                                                            10,std::bind(&ImageProcessingNode::camera_image_depht_callback,this,std::placeholders::_1));
    //Create publisher
    debug_image_publisher_ = this->create_publisher<sensor_msgs::msg::Image>("/camera/image_debug",10);
    ball_height_publisher_ = this->create_publisher<std_msgs::msg::Int32>("camera/ball_height_traking",10);
  }

private:
    //DEBUG?
    bool DEBUGMOD = false;

    //Comunicação ROS
    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr ball_height_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::Image>::SharedPtr debug_image_publisher_;

    rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr realsense_image_depth_subscriber_;

    sensor_msgs::msg::Image::SharedPtr debug_image_msg_;
    cv_bridge::CvImagePtr cv_ptr_depth;

    //Definições da depth image
    cv::Mat realsense_image_depth;
    int image_width = 640;
    int image_height = 480;

    //Região onde a bola pode ser detectada ball_ref_dist-ball_ref_bound < ball_dist < ball_ref_dist+ball_ref_bound
    int ball_ref_dist = 910;
    int ball_ref_bound = 100;

    //Vetor para guardar a posição da bola detectada pela transformada de Hough
    std::vector<cv::Vec3f> circles;
    
    //Posição e profundidade da bola detectada
    int ball_depth = ball_ref_dist;
    int ball_cx;
    int ball_cy;

    int ball_height;
    std_msgs::msg::Int32 ball_height_msg;

    //Intrinsic matrix param to Tsai's Algorithm
    double fx = 609.9407348632812;
    double cx = 324.9618835449219;
    double fy = 609.0489501953125;
    double cy = 242.5819854736328;

    
    void camera_image_depht_callback(const sensor_msgs::msg::Image::SharedPtr msg)
    {
      //Recebe a imagem do depth da Realsense
      try
      {
        cv_ptr_depth = cv_bridge::toCvCopy(msg,sensor_msgs::image_encodings::TYPE_16UC1);
      }
      catch (cv_bridge::Exception& e)
      {
        RCLCPP_ERROR(this->get_logger(),"cv_bridge exception: %s", e.what());
      }

      realsense_image_depth = cv_ptr_depth->image;

      //Determina a posição X,Y da bola na imagem do depth
      get_ball_position();
      //Determina a posição real Y da bola
      get_ball_y_position();
      //Publica para o controle a posição real Y da bola
      publishe_ball_y_position();
    }

    void get_ball_position()
    {
      cv::Mat gray_image_to_hough_transform(realsense_image_depth.size(),CV_8UC1,cv::Scalar(255));

      for(int i=0;i<image_width;i++)
      {
        for(int j=0;j<image_height;j++)
        {
          int ball_depth_aux = cv_ptr_depth->image.at<u_int16_t>(j,i);
          if(ball_depth_aux < ball_ref_dist+ball_ref_bound && ball_depth_aux > ball_ref_dist-ball_ref_bound)
          {
            gray_image_to_hough_transform.at<uchar>(j,i) = 0;
          }
        }
      }

      HoughCircles(gray_image_to_hough_transform, circles, cv::HOUGH_GRADIENT, 1,
                 gray_image_to_hough_transform.rows/8,  
                 100, 8, 10,40);
      
      get_center_depth();
      

      if(DEBUGMOD)
      {
        //RCLCPP_INFO(this->get_logger(),"circles detct = %d",circles.size());

        cv::Mat image_debug;
        cv::cvtColor(gray_image_to_hough_transform,image_debug,CV_GRAY2BGR);

        for( size_t i = 0; i < circles.size(); i++ )
        {
          cv::Vec3i c = circles[i];
          cv::Point center = cv::Point(c[0], c[1]);
          // circle center
          circle( image_debug, center, 1, cv::Scalar(0,100,100), 3, cv::LINE_AA);
          // circle outline
          int radius = c[2];
          circle( image_debug, center, radius, cv::Scalar(255,0,255), 3, cv::LINE_AA);
        }

        debug_image_msg_ = cv_bridge::CvImage(std_msgs::msg::Header(),"bgr8",image_debug).toImageMsg();
        debug_image_publisher_->publish(*debug_image_msg_.get());
      }
      circles.clear();
    }

    void get_center_depth()
    {
      if(!circles.empty())
      {
        cv::Vec3i c = circles[0];
        int ball_depth_aux = cv_ptr_depth->image.at<u_int16_t>(c[1],c[0]);
        if(ball_depth_aux < ball_ref_dist+ball_ref_bound && ball_depth_aux > ball_ref_dist-ball_ref_bound)
        {
          ball_depth = ball_depth_aux;
          ball_cx = c[0];
          ball_cy = c[1];
        }
        //if(DEBUGMOD)RCLCPP_INFO(this->get_logger(),"Dist ball: %f posicao: x %d, y %d",0.001*(float)ball_depth,ball_cx,ball_cy);
      }

    }

    void get_ball_y_position()
    {

      ball_height = (int)((double)ball_depth*((double)ball_cy-cy)/fy);

      if(DEBUGMOD)RCLCPP_INFO(this->get_logger(),"Ball Height: %f ",0.001*(float)ball_height);
    }

    void publishe_ball_y_position()
    {
      ball_height_msg.data = ball_height;
      ball_height_publisher_->publish(ball_height_msg);
    }
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ImageProcessingNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}