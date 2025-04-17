//
//  main.cpp
//  RMSE
//
//  Created by Luis Paulo Santos on 01/04/2025.
//

#include <iostream>
#include "ImagePPM.hpp"
#include <math.h>


static void error_message (void) {
    fprintf (stderr,"Utilization: RMSE <img1-fn.pmm> <ref_img-fn.pmm> [<gamma>] [<out-fn.pmm>]\n");
    fprintf (stderr,"\t <img1-fn.pmm> and <ref_img-fn.pmm> ,must have the same dimensions\n");
    fprintf (stderr,"\t Default gamma=0.5 \n");
    fprintf (stderr,"\t Default output filname=\"RMSE.ppm\" \n");
}

static void RMSE (ImagePPM in, ImagePPM in_ref, ImagePPM out, float& RMSE, float& MinMaxScaledRMSE, float& averageY, float& minY, float& maxY) {
    
    // compute Y average and min and max
    averageY=0.f;
    maxY = minY = in_ref.get(0, 0).Y();
    for (int y=0 ; y<in_ref.H ; y++) {
        for (int x=0 ; x<in_ref.W ; x++) {
            float const Y = in_ref.get(x,y).Y();
            averageY += Y;
            if (Y>maxY) maxY = Y;
            if (Y<minY) minY = Y;
        }
    }
    averageY /= (in_ref.W*in_ref.H);
    float const rangeY = maxY - minY;
    
    float RMSE_sum =0.f;
    float MinMaxRMSE_sum =0.f;
    for (int y=0 ; y<in_ref.H ; y++) {
        for (int x=0 ; x<in_ref.W ; x++) {
            float const Y_ref = in_ref.get(x,y).Y();
            float const Y_in = in.get(x,y).Y();
            float const SE = (Y_ref-Y_in)*(Y_ref-Y_in);
            out.set(x, y, RGB(SE,SE,SE));
            RMSE_sum += SE;
            float const MinMaxY_ref = (Y_ref-minY)/rangeY;
            float const MinMaxY_in = (Y_in-minY)/rangeY;
            float const MinMaxSE = (MinMaxY_ref-MinMaxY_in)*(MinMaxY_ref-MinMaxY_in);
            MinMaxRMSE_sum += MinMaxSE;
        }
    }
    RMSE = sqrt(RMSE_sum)/(in_ref.W*in_ref.H);
    MinMaxScaledRMSE = sqrt(MinMaxRMSE_sum)/(in_ref.W*in_ref.H);
}

int main(int argc, const char * argv[]) {
   
    if (argc<3) {
        error_message ();
        return  0;
    }
    
    std::string img_in_fn(argv[1]);
    std::string img_ref_fn(argv[2]);
    float gamma = (argc>=4 ? atof(argv[3]) : 0.5f);
    std::string img_out_fn(argc>=5 ? argv[4] : "RMSE.ppm");
    ImagePPM img_in, img_ref;
    
    if (!img_in.Load(img_in_fn)) return 0;
    if (!img_ref.Load(img_ref_fn)) return 0;
    
    if (img_ref.W != img_in.W || img_in.H != img_ref.H) {
        fprintf (stderr, "The 2 input images have different sizes!\n");
        return 0;
    }
    ImagePPM out(img_ref.W, img_ref.H);
    
    float RMSEY, MinMaxScaledRMSEY, averageY, minY, maxY;

    RMSE (img_in, img_ref, out, RMSEY, MinMaxScaledRMSEY, averageY, minY,maxY);
    out.MonoGammaCorrect(gamma);
    
    out.Save(img_out_fn);
    
    fprintf (stdout, "Reference Image : %s \n", img_ref_fn.c_str());
    fprintf (stdout, "\tminY = %f, maxY = %f, average Y = %f\n", minY, maxY, averageY);
    fprintf (stdout, "\tW=%d, H=%d\n", img_ref.W, img_ref.H);
    fprintf (stdout, "ImageIn : %s\n", img_in_fn.c_str());
    fprintf (stdout, "RMSE = %f, MinMaxScaledRMSE = %f\n", RMSEY, MinMaxScaledRMSEY);
    fprintf (stdout, "Image Out : %s (gamma=%.2f)\n", img_out_fn.c_str(), gamma);

    return 1;
}
