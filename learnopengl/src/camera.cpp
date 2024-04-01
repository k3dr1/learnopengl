#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default values
#define DEFAULT_YAW -90.0f
#define DEFAULT_PITCH 0.0f
#define DEFAULT_SPEED 2.5f
#define DEFAULT_SENSITIVITY 0.1f
#define DEFAULT_ZOOM 45.0f

// An abstract camera class that processes input and calculates
// the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
  public:
    // member variables
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    // constructor with vectors
    // given the camera's position in the world
    // and the world's up direction,
    // constructs a camera looking at given yaw and pitch
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH)
        : position(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(worldUp), yaw(yaw), pitch(pitch),
          movementSpeed(DEFAULT_SPEED), mouseSensitivity(DEFAULT_SENSITIVITY), zoom(DEFAULT_ZOOM)
    {
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : position(glm::vec3(posX, posY, posZ)), front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(glm::vec3(upX, upY, upZ)),
          yaw(yaw), pitch(pitch), movementSpeed(DEFAULT_SPEED), mouseSensitivity(DEFAULT_SENSITIVITY),
          zoom(DEFAULT_ZOOM)
    {
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + front, up);
    }

    // processes input received from any keyboard-like input system
    // Accepts input parameter in the form of camera defined ENUM (to abstract it
    // from windowing systems)
    void processKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = movementSpeed * deltaTime;
        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
    }

    // processes input received from a mouse input system
    // Expects the offset value in both the x and y direction.
    void processMouseMovement(double xoffset, double yoffset, GLboolean invertPitch = true,
                              GLboolean constrainPitch = true)
    {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires
    // input on the vertical wheel-axis
    void processMouseScroll(double yoffset, GLboolean constrainZoom = true)
    {
        zoom -= (float)yoffset;
        if (!constrainZoom)
            return;
        if (zoom < 1.0f)
            zoom = 1.0f;
        if (zoom > 45.0f)
            zoom = 45.0f;
    }

  private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 new_front;
        new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        new_front.y = sin(glm::radians(pitch));
        new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(new_front);
        // also re-calculate the Right and Up vector
        // we should normalize the vectors, because their length gets closer to 0
        // the more you look up or down which results in slower movement.
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

#undef DEFAULT_YAW
#undef DEFAULT_PITCH
#undef DEFAULT_SPEED
#undef DEFAULT_SENSITIVITY
#undef DEFAULT_ZOOM
#endif
