//
//  loadModel.h
//  vulkan-tutorial
//
//  Created by Alejandro Ball on 21/12/2021.
//

#ifndef loadModel_h
#define loadModel_h

#include "geninc.h"

void loadModel(struct DrawingData** vertices, uint64_t* num_vertices, uint32_t** indices, uint64_t* num_indices, const char* MODEL_PATH);

#endif /* loadModel_h */
