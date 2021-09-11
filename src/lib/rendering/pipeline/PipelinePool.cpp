/* PIPELINE POOL.cpp
 *   by Lut99
 *
 * Created:
 *   11/09/2021, 14:06:26
 * Last edited:
 *   11/09/2021, 14:06:26
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Contains the PipelinePool, which is a class that is in charge of
 *   managing and, more importantly, spawning new pipelines in an efficient
 *   way.
**/

#include "tools/Logger.hpp"
#include "../auxillary/ErrorCodes.hpp"

#include "PipelinePool.hpp"

using namespace std;
using namespace Makma3D;
using namespace Makma3D::Rendering;


/***** PIPELINEPOOL CLASS *****/
/* Constructor for the PipelinePool class, which takes a GPU where all the pipelines will live. */
PipelinePool::PipelinePool(const Rendering::GPU& gpu) :
    gpu(gpu)
{
    logger.logc(Verbosity::important, PipelinePool::channel, "Initializing...");


    
    logger.logc(Verbosity::important, PipelinePool::channel, "Init success.");
}

/* Copy constructor for the PipelinePool class. */
PipelinePool::PipelinePool(const PipelinePool& other) :
    gpu(other.gpu)
{
    logger.logc(Verbosity::debug, PipelinePool::channel, "Copying...");


    
    logger.logc(Verbosity::debug, PipelinePool::channel, "Copy success.");
}

/* Move constructor for the PipelinePool class. */
PipelinePool::PipelinePool(PipelinePool&& other) :
    gpu(other.gpu)
{

}

/* Destructor for the PipelinePool class. */
PipelinePool::~PipelinePool() {
    logger.logc(Verbosity::important, PipelinePool::channel, "Cleaning...");


    
    logger.logc(Verbosity::important, PipelinePool::channel, "Cleaned.");
}



/* Swap operator for the PipelinePool class. */
void Rendering::swap(PipelinePool& pp1, PipelinePool& pp2) {
    #ifndef NDEBUG
    if (pp1.gpu != pp2.gpu) { logger.fatalc(PipelinePool::channel, "Cannot swap pipeline pools with different GPUs."); }
    #endif

    using std::swap;

}
