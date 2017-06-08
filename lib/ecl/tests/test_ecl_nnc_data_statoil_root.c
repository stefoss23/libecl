/*
   Copyright (C) 2017  Statoil ASA, Norway.

   The file 'test_ecl_nnc_data_statoil.c' is part of ERT - Ensemble based Reservoir Tool.

   ERT is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   ERT is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.

   See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html>
   for more details.
*/

#include <ert/ecl/ecl_nnc_data.h>
#include <ert/ecl/ecl_kw_magic.h>
#include <ert/ecl/ecl_nnc_geometry.h>

#include <ert/ecl/ecl_endian_flip.h>
#include <ert/ecl/ecl_file.h>
#include <ert/ecl/ecl_grid.h>
#include <ert/ecl/ecl_kw.h>

#include <ert/util/util.h>
#include <ert/util/test_util.h>
#include <ert/util/test_work_area.h>

void probe_nnc_geometry(ecl_nnc_geometry_type * nnc_geo) {
   int nnc_size = ecl_nnc_geometry_size(nnc_geo);
   int current_grid1 = -1;
   int current_grid2 = -1;
   int num = 0;
   printf(" *************** SIZE: %d\n", nnc_size);
   for (int n = 0; n < nnc_size; n++) {
      ecl_nnc_pair_type * pair = ecl_nnc_geometry_iget( nnc_geo, n );
      
      if (pair->grid_nr1 != current_grid1 || pair->grid_nr2 != current_grid2) {
         printf(" ******************* %d: g1, g2 = %d, %d, num: %d\n", n, current_grid1, current_grid2, num);
         current_grid1 = pair->grid_nr1;
         current_grid2 = pair->grid_nr2;
         num = 1;
      }
      else {
         num++;
      }
   }
}

void probe_file(ecl_file_view_type * view_file) {
   const int file_num_kw = ecl_file_view_get_size( view_file);
   for (int n = 0; n < file_num_kw; n++) {
      ecl_kw_type * ecl_kw = ecl_file_view_iget_kw( view_file , n );
      const char *current_kw = ecl_kw_get_header(ecl_kw);
      printf(" ************ %d: KEYWORD: %s", n, current_kw);
      if (strcmp( LGRHEADI_KW , current_kw) == 0) {
         int m = ecl_kw_iget_int( ecl_kw , LGRHEADI_LGR_NR_INDEX);
         printf(" ************************* %d", m);
      }
      if (strcmp( "FLROILL+", current_kw) == 0) {
         int m = ecl_kw_get_size(ecl_kw); 
         printf(" SIZE: %d", m);   
      }
      printf("\n");
   }
}

//GENERALIZE
void assert_data_values_read(ecl_nnc_data_type * nnc_data) {
   int data_size = ecl_nnc_data_get_size(nnc_data);
   for (int n = 0; n < data_size; n++) {
      if (true) 
         test_assert_double_not_equal(-1.0, ecl_nnc_data_iget_value(nnc_data, n));
   }
}

int find_index(ecl_nnc_geometry_type * nnc_geo, int grid1, int grid2, int indx1, int indx2) {
   int index = -1;
   int nnc_size = ecl_nnc_geometry_size( nnc_geo );
   for (int n = 0; n < nnc_size; n++) {
      ecl_nnc_pair_type * pair = ecl_nnc_geometry_iget( nnc_geo, n );
      if (pair->grid_nr1 == grid1 && pair->grid_nr2 == grid2)
          if (pair->global_index1 == indx1 && pair->global_index2 ==indx2) {
          index = n;
          break;
      }
   }
   return index;
}


void test_alloc_file_tran(char * filename) {
   char * grid_file_name = ecl_util_alloc_filename(NULL , filename , ECL_EGRID_FILE , false  , -1);
   char * init_file_name = ecl_util_alloc_filename(NULL , filename , ECL_INIT_FILE , false  , -1);
   ecl_file_type * init_file = ecl_file_open( init_file_name , 0 );
   ecl_grid_type * grid = ecl_grid_alloc( grid_file_name );
   ecl_nnc_geometry_type * nnc_geo = ecl_nnc_geometry_alloc( grid );
   ecl_file_view_type * view_file = ecl_file_get_global_view( init_file );

   ecl_nnc_data_type * nnc_geo_data = ecl_nnc_data_alloc_tran(grid, nnc_geo, view_file);
   
   //These numerical values are hand-tuned the specific input file at:
   //${_eclpath}/Troll/MSW_LGR/2BRANCHES-CCEWELLPATH-NEW-SCH-TUNED-AR3
   int index;
   index = find_index( nnc_geo, 0, 0, 541, 14507);
   test_assert_double_equal(13.784438, ecl_nnc_data_iget_value( nnc_geo_data, index) );   

   index = find_index( nnc_geo, 0, 0, 48365, 118191);
   test_assert_double_equal(0.580284 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   index = find_index( nnc_geo, 0, 19, 42830, 211);
   test_assert_double_equal(0.571021 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   index = find_index( nnc_geo, 0, 79, 132406, 76);
   test_assert_double_equal(37.547710 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   index = find_index( nnc_geo, 18, 12, 303, 115);
   test_assert_double_equal(0.677443 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   index = find_index( nnc_geo, 72, 71, 255, 179);
   test_assert_double_equal(0.045813 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   index = find_index( nnc_geo, 110, 109, 271, 275);
   test_assert_double_equal(16.372242 , ecl_nnc_data_iget_value( nnc_geo_data, index) );

   ecl_nnc_data_free(nnc_geo_data);
   ecl_nnc_geometry_free(nnc_geo);
   ecl_grid_free(grid);
   ecl_file_close(init_file);
   free(grid_file_name);
   free(init_file_name);

}


void test_alloc_file_flux(char * filename) {
   char * grid_file_name = ecl_util_alloc_filename(NULL , filename , ECL_EGRID_FILE , false  , -1);
   char * init_file_name = ecl_util_alloc_filename(NULL , filename , ECL_RESTART_FILE , false  , 0);

   ecl_file_type * init_file = ecl_file_open( init_file_name , 0 );
   ecl_grid_type * grid = ecl_grid_alloc( grid_file_name );
   ecl_nnc_geometry_type * nnc_geo = ecl_nnc_geometry_alloc( grid );
  
   probe_nnc_geometry(nnc_geo);
   {   
      ecl_file_view_type * view_file = ecl_file_get_global_view( init_file );
      //probe_file ( view_file );

      ecl_nnc_data_type * nnc_flux_data = ecl_nnc_data_alloc_flux(grid, nnc_geo, view_file);
      assert_data_values_read(nnc_flux_data);
      ecl_nnc_data_free( nnc_flux_data );
   }
   ecl_nnc_geometry_free(nnc_geo);
   ecl_grid_free(grid);
   ecl_file_close(init_file);
   free(grid_file_name);
   free(init_file_name);
}


int main(int argc , char ** argv) {
   test_alloc_file_tran(argv[1]);
   test_alloc_file_flux(argv[2]);
   return 0;
}
