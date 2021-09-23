/* SAMPLER.cpp
 *   by Lut99
 *
 * Created:
 *   25/08/2021, 16:46:36
 * Last edited:
 *   25/08/2021, 16:46:36
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the Sampler class, which wraps the a VkSampler object. Used
 *   to allow access to textures in shaders.
**/

#include "Sampler.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** SAMPLER CLASS *****/
/* Constructor for the Sampler class, which takes the GPU where the Sampler lives and the vk_sampler object to wrap. */
Sampler::Sampler(const Rendering::GPU& gpu, const VkSampler& vk_sampler) :
    gpu(gpu),
    vk_sampler(vk_sampler)
{}


/* Destructor for the Sampler class. */
Sampler::~Sampler() {}
