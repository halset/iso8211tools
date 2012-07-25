/******************************************************************************
 * $Id: 8211dump.cpp 10645 2007-01-18 02:22:39Z warmerdam $
 *
 * Project:  SDTS Translator
 * Purpose:  Dump 8211 file in verbose form - just a junk program. 
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#include <stdio.h>
#include "iso8211.h"
#include "cpl_vsi.h"

CPL_CVSID("$Id: 8211dump.cpp 10645 2007-01-18 02:22:39Z warmerdam $");


int main( int nArgc, char ** papszArgv )

{
    DDFModule   oModule;
    const char  *pszFilename = NULL;
    int         bFSPTHack = FALSE;

/* -------------------------------------------------------------------- */
/*      Check arguments.                                                */
/* -------------------------------------------------------------------- */
    for( int iArg = 1; iArg < nArgc; iArg++ )
    {
        if( EQUAL(papszArgv[iArg],"-fspt_repeating") )
            bFSPTHack = TRUE;
        else
            pszFilename = papszArgv[iArg];
    }

    if( pszFilename == NULL )
    {
        printf( "Usage: catalogdump filename\n" );
        exit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Open file.                                                      */
/* -------------------------------------------------------------------- */
    if( !oModule.Open( pszFilename ) )
        exit( 1 );

/* -------------------------------------------------------------------- */
/*      Apply FSPT hack if required.                                    */
/* -------------------------------------------------------------------- */
    if( bFSPTHack )
    {
        DDFFieldDefn *poFSPT = oModule.FindFieldDefn( "FSPT" );

        if( poFSPT == NULL )
            fprintf( stderr, 
                     "unable to find FSPT field to set repeating flag.\n" );
        else
            poFSPT->SetRepeatingFlag( TRUE );
    }

/* -------------------------------------------------------------------- */
/*      Dump all recodrs.                                   */
/* -------------------------------------------------------------------- */
    DDFRecord       *poRecord;
    DDFField *paoField;
    long nStartLoc;
    
    int         nMaxRepeat = 8;
    if( getenv("DDF_MAXDUMP") != NULL )
        nMaxRepeat = atoi(getenv("DDF_MAXDUMP"));


    nStartLoc = VSIFTellL( oModule.GetFP() );
    for( poRecord = oModule.ReadRecord();
         poRecord != NULL; poRecord = oModule.ReadRecord() )
    {

        
        for( int i = 0; i < poRecord->GetFieldCount(); i++ ) 
        {
             paoField = poRecord->GetField( i );
             
             DDFFieldDefn *poFieldDefn = paoField->GetFieldDefn();
             printf("<%s>\n", poFieldDefn->GetName() );

             int iOffset = 0;
             
             for( int nLoopCount = 0; nLoopCount < paoField->GetRepeatCount(); nLoopCount++ )
             {
                if( nLoopCount > nMaxRepeat )
                {
                    break;
                }
                
                for( int sbfi = 0; sbfi < poFieldDefn->GetSubfieldCount(); sbfi++ )
                {
                    int         nBytesConsumed;
                    
                    printf(" <%s>", poFieldDefn->GetSubfield(sbfi)->GetName());
        
                    /*
                    poFieldDefn->GetSubfield(sbfi)->DumpData( paoField->GetData()+iOffset,
                                                      paoField->GetDataSize() - iOffset, stdout );
                    */
                    printf("%s", poFieldDefn->GetSubfield(sbfi)->ExtractStringData( paoField->GetData()+iOffset,
                                                      paoField->GetDataSize() - iOffset, NULL ));
                    
                    printf("</%s>\n", poFieldDefn->GetSubfield(sbfi)->GetName());
                
                    poFieldDefn->GetSubfield(sbfi)->GetDataLength( paoField->GetData()+iOffset,
                                                           paoField->GetDataSize() - iOffset,
                                                           &nBytesConsumed );
        
                    iOffset += nBytesConsumed;
                }
            }

             
             
             printf("</%s>\n", poFieldDefn->GetName() );
        }

        nStartLoc = VSIFTellL( oModule.GetFP() );
    }

    oModule.Close();
    
#ifdef DBMALLOC
    malloc_dump(1);
#endif

}



