#include "camera.h"
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}
glm::mat4 Camera::GetProjMatrix() const
{
    return glm::perspective(glm::radians(Zoom), 800.0f / 600.0f, 0.1f, 200.0f);
}
glm::mat4 OrthoCamera::GetViewMatrix() const
{
    return glm::lookAt(Position, lookatpos, Up);
}
glm::mat4 OrthoCamera::GetProjMatrix() const
{
    return glm::ortho(left, right, bottom, top, near, far);
}