/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name(s) of the copyright holder(s) nor the names of its
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

#ifndef BLIS_L3_UKR_PROT_H
#define BLIS_L3_UKR_PROT_H

//
// Define template prototypes for level-3 micro-kernels.
//

#undef  L3TPROT
#define L3TPROT( ctype, ch, funcname, opname ) \
\
void PASTEMAC(ch,funcname) \
     ( \
       PASTECH(opname,_params), \
       BLIS_AUXINFO_PARAM, \
       BLIS_CNTX_PARAM  \
     );

#define GEMM_UKR2_PROT( ctypeab, ctypec, chab, chc, fn ) L3TPROT( /* not used */, PASTECH(chab,chc), fn, gemm );

#define GEMM_UKR_PROT(     ctype, ch, fn )  L3TPROT( ctype, ch, fn, gemm );
#define GEMMTRSM_UKR_PROT( ctype, ch, fn )  L3TPROT( ctype, ch, fn, gemmtrsm );
#define TRSM_UKR_PROT(     ctype, ch, fn )  L3TPROT( ctype, ch, fn, trsm );


#endif

