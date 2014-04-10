/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"

extern scalm_t*   scalm_cntl;

blksz_t*          gemm4m_mc;
blksz_t*          gemm4m_nc;
blksz_t*          gemm4m_kc;
blksz_t*          gemm4m_mr;
blksz_t*          gemm4m_nr;
blksz_t*          gemm4m_kr;

func_t*           gemm4m_ukrs;

packm_t*          gemm4m_packa_cntl;
packm_t*          gemm4m_packb_cntl;

gemm_t*           gemm4m_cntl_bp_ke;
gemm_t*           gemm4m_cntl_op_bp;
gemm_t*           gemm4m_cntl_mm_op;
gemm_t*           gemm4m_cntl_vl_mm;

gemm_t*           gemm4m_cntl;


void bli_gemm4m_cntl_init()
{
	// Create blocksize objects for each dimension.
	gemm4m_mc
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_MC_C, BLIS_EXTEND_4M_MC_C,
	                      BLIS_DEFAULT_4M_MC_Z, BLIS_EXTEND_4M_MC_Z );
	gemm4m_nc
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_NC_C, BLIS_EXTEND_4M_NC_C,
	                      BLIS_DEFAULT_4M_NC_Z, BLIS_EXTEND_4M_NC_Z );
	gemm4m_kc
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_KC_C, BLIS_EXTEND_4M_KC_C,
	                      BLIS_DEFAULT_4M_KC_Z, BLIS_EXTEND_4M_KC_Z );
	gemm4m_mr
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_MR_C, BLIS_EXTEND_4M_MR_C,
	                      BLIS_DEFAULT_4M_MR_Z, BLIS_EXTEND_4M_MR_Z );
	gemm4m_nr
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_NR_C, BLIS_EXTEND_4M_NR_C,
	                      BLIS_DEFAULT_4M_NR_Z, BLIS_EXTEND_4M_NR_Z );
	gemm4m_kr
	=
	bli_blksz_obj_create( 0,                    0,
	                      0,                    0,
	                      BLIS_DEFAULT_4M_KR_C, BLIS_EXTEND_4M_KR_C,
	                      BLIS_DEFAULT_4M_KR_Z, BLIS_EXTEND_4M_KR_Z );



	// Create function pointer object for each datatype-specific gemm
	// micro-kernel.
	gemm4m_ukrs = bli_func_obj_create( NULL,
	                                   NULL,
	                                   BLIS_CGEMM4M_UKERNEL,
	                                   BLIS_ZGEMM4M_UKERNEL );


	// Create control tree objects for packm operations.
	gemm4m_packa_cntl
	=
	bli_packm_cntl_obj_create( BLIS_BLOCKED,
	                           BLIS_VARIANT4,
	                           gemm4m_mr,
	                           gemm4m_kr,
	                           TRUE,  // densify; used by hemm/symm
	                           FALSE, // do NOT invert diagonal
	                           FALSE, // reverse iteration if upper?
	                           FALSE, // reverse iteration if lower?
	                           BLIS_PACKED_ROW_PANELS_4M,
	                           BLIS_BUFFER_FOR_A_BLOCK );

	gemm4m_packb_cntl
	=
	bli_packm_cntl_obj_create( BLIS_BLOCKED,
	                           BLIS_VARIANT4,
	                           gemm4m_kr,
	                           gemm4m_nr,
	                           TRUE,  // densify; used by hemm/symm
	                           FALSE, // do NOT invert diagonal
	                           FALSE, // reverse iteration if upper?
	                           FALSE, // reverse iteration if lower?
	                           BLIS_PACKED_COL_PANELS_4M,
	                           BLIS_BUFFER_FOR_B_PANEL );


	//
	// Create a control tree for packing A and B, and streaming C.
	//

	// Create control tree object for lowest-level block-panel kernel.
	gemm4m_cntl_bp_ke
	=
	bli_gemm_cntl_obj_create( BLIS_UNB_OPT,
	                          BLIS_VARIANT2,
	                          NULL,
	                          gemm4m_ukrs,
	                          NULL, NULL, NULL,
	                          NULL, NULL, NULL );

	// Create control tree object for outer panel (to block-panel)
	// problem.
	gemm4m_cntl_op_bp
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          BLIS_VARIANT1,
	                          gemm4m_mc,
	                          NULL,
	                          NULL,
	                          gemm4m_packa_cntl,
	                          gemm4m_packb_cntl,
	                          NULL,
	                          gemm4m_cntl_bp_ke,
	                          NULL );

	// Create control tree object for general problem via multiple
	// rank-k (outer panel) updates.
	gemm4m_cntl_mm_op
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          BLIS_VARIANT3,
	                          gemm4m_kc,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          gemm4m_cntl_op_bp,
	                          NULL );

	// Create control tree object for very large problem via multiple
	// general problems.
	gemm4m_cntl_vl_mm
	=
	bli_gemm_cntl_obj_create( BLIS_BLOCKED,
	                          BLIS_VARIANT2,
	                          gemm4m_nc,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          NULL,
	                          gemm4m_cntl_mm_op,
	                          NULL );

	// Alias the "master" gemm control tree to a shorter name.
	gemm4m_cntl = gemm4m_cntl_vl_mm;

}

void bli_gemm4m_cntl_finalize()
{
	bli_blksz_obj_free( gemm4m_mc );
	bli_blksz_obj_free( gemm4m_nc );
	bli_blksz_obj_free( gemm4m_kc );
	bli_blksz_obj_free( gemm4m_mr );
	bli_blksz_obj_free( gemm4m_nr );
	bli_blksz_obj_free( gemm4m_kr );

	bli_func_obj_free( gemm4m_ukrs );

	bli_cntl_obj_free( gemm4m_packa_cntl );
	bli_cntl_obj_free( gemm4m_packb_cntl );

	bli_cntl_obj_free( gemm4m_cntl_bp_ke );
	bli_cntl_obj_free( gemm4m_cntl_op_bp );
	bli_cntl_obj_free( gemm4m_cntl_mm_op );
	bli_cntl_obj_free( gemm4m_cntl_vl_mm );
}

