# Texconv

Example usage:

`texconv -pow2 -f BC1_UNORM cat.jpg`

This loads a JPEG image 'cat.jpg', resizes the image to a power of 2 in each dimension (if the original was 512 x 683 it is resized to 256 x 512), mipmaps are generated, the file is converted to `DXGI_FORMAT_BC1_UNORM` (aka DXT1) block compression, and written out as 'cat.dds'.

More here: https://github.com/Microsoft/DirectXTex/wiki/Texconv
