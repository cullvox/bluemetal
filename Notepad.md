Viewports are an idea of a texture that needs to get rendered to.

Most viewports will probably get rendered off screen at some point and then that image will be transformed or used
in some way to present the image.

* Viewports are color images always.

Render passes will run on viewport images, render passes usually may contain their own images. 
Importantly, if a render pass requires an image it will probably have to be resized to the viewport size.


