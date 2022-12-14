/*****************************************************************
|
|    AP4 - Atom Factory
|
|    Copyright 2002 Gilles Boccon-Gibod
|
|
|    This file is part of Bento4/AP4 (MP4 Atom Processing Library).
|
|    Unless you have obtained Bento4 under a difference license,
|    this version of Bento4 is Bento4|GPL.
|    Bento4|GPL is free software; you can redistribute it and/or modify
|    it under the terms of the GNU General Public License as published by
|    the Free Software Foundation; either version 2, or (at your option)
|    any later version.
|
|    Bento4|GPL is distributed in the hope that it will be useful,
|    but WITHOUT ANY WARRANTY; without even the implied warranty of
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|    GNU General Public License for more details.
|
|    You should have received a copy of the GNU General Public License
|    along with Bento4|GPL; see the file COPYING.  If not, write to the
|    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
|    02111-1307, USA.
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include "Ap4.h"
#include "Ap4AtomFactory.h"
#include "Ap4SampleEntry.h"
#include "Ap4IsmaCryp.h"
#include "Ap4UrlAtom.h"
#include "Ap4MoovAtom.h"
#include "Ap4MvhdAtom.h"
#include "Ap4TrakAtom.h"
#include "Ap4HdlrAtom.h"
#include "Ap4DrefAtom.h"
#include "Ap4TkhdAtom.h"
#include "Ap4MdhdAtom.h"
#include "Ap4StsdAtom.h"
#include "Ap4StscAtom.h"
#include "Ap4StcoAtom.h"
#include "Ap4Co64Atom.h"
#include "Ap4StszAtom.h"
#include "Ap4EsdsAtom.h"
#include "Ap4SttsAtom.h"
#include "Ap4CttsAtom.h"
#include "Ap4StssAtom.h"
#include "Ap4FtypAtom.h"
#include "Ap4VmhdAtom.h"
#include "Ap4SmhdAtom.h"
#include "Ap4NmhdAtom.h"
#include "Ap4HmhdAtom.h"
#include "Ap4SchmAtom.h"
#include "Ap4FrmaAtom.h"
#include "Ap4TimsAtom.h"
#include "Ap4RtpAtom.h"
#include "Ap4SdpAtom.h"
#include "Ap4IkmsAtom.h"
#include "Ap4IsfmAtom.h"
#include "Ap4TrefTypeAtom.h"
#include "Ap4AvcCAtom.h"
#include "Ap4FtabAtom.h"
#include "Ap4ChplAtom.h"
#include "Ap4DataAtom.h"
#include "Ap4DcomAtom.h"
#include "Ap4CmvdAtom.h"

/*----------------------------------------------------------------------
|       class variables
+---------------------------------------------------------------------*/
AP4_AtomFactory AP4_AtomFactory::DefaultFactory;

/*----------------------------------------------------------------------
|       AP4_AtomFactory::AddTypeHandler
+---------------------------------------------------------------------*/
AP4_Result
AP4_AtomFactory::AddTypeHandler(TypeHandler* handler)
{
    return m_TypeHandlers.Add(handler);
}

/*----------------------------------------------------------------------
|       AP4_AtomFactory::RemoveTypeHandler
+---------------------------------------------------------------------*/
AP4_Result
AP4_AtomFactory::RemoveTypeHandler(TypeHandler* handler)
{
    return m_TypeHandlers.Remove(handler);
}

/*----------------------------------------------------------------------
|       AP4_AtomFactory::CreateAtomFromStream
+---------------------------------------------------------------------*/
AP4_Result
AP4_AtomFactory::CreateAtomFromStream(AP4_ByteStream& stream, 
                                      AP4_Atom*&      atom)
{
    AP4_Size bytes_available = 0;
    if (AP4_FAILED(stream.GetSize(bytes_available)) ||
        bytes_available == 0) {
        bytes_available = (AP4_Size)((unsigned long)(-1));
    }
    return CreateAtomFromStream(stream, bytes_available, atom, NULL);
}

/*----------------------------------------------------------------------
|       AP4_AtomFactory::CreateAtomFromStream
+---------------------------------------------------------------------*/
AP4_Result
AP4_AtomFactory::CreateAtomFromStream(AP4_ByteStream& stream, 
                                      AP4_Size&       bytes_available,
                                      AP4_Atom*&      atom,
									  AP4_Atom*	      parent)
{
    AP4_Result result;

    // NULL by default
    atom = NULL;

    // check that there are enough bytes for at least a header
    if (bytes_available < 8) return AP4_ERROR_EOS;

    // remember current stream offset
    AP4_Offset start;
    stream.Tell(start);

    // read atom size
    AP4_UI32 size;
    result = stream.ReadUI32(size);
    if (AP4_FAILED(result)) {
        stream.Seek(start);
        return result;
    }

    if (size == 0) {
        // atom extends to end of file
        AP4_Size streamSize = 0;
        stream.GetSize(streamSize);
        if (streamSize >= start) {
            size = streamSize - start;
        }
    }

    // check the size (we don't handle extended size yet)
    if (size != 1 && size < 8 || size > bytes_available) {
        stream.Seek(start);
        return AP4_ERROR_INVALID_FORMAT;
    }

    // read atom type
    AP4_Atom::Type type;
    result = stream.ReadUI32(type);
    if (AP4_FAILED(result)) {
        stream.Seek(start);
        return result;
    }

	if (size == 1)
	{
		AP4_UI32 size_high;

		result = stream.ReadUI32(size_high);
		if (AP4_FAILED(result) || size_high) {
			stream.Seek(start);
			return AP4_ERROR_INVALID_FORMAT;
		}

		result = stream.ReadUI32(size);
		if (AP4_FAILED(result)) {
			stream.Seek(start);
			return result;
		}
	}

    // create the atom
    switch (type) {
      case AP4_ATOM_TYPE_MOOV:
        atom = new AP4_MoovAtom(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_MVHD:
        atom = new AP4_MvhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_TRAK:
        atom = new AP4_TrakAtom(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_HDLR:
        atom = new AP4_HdlrAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_DREF:
        atom = new AP4_DrefAtom(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_URL:
        atom = new AP4_UrlAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_TKHD:
        atom = new AP4_TkhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_MDHD:
        atom = new AP4_MdhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_STSD:
        atom = new AP4_StsdAtom(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_STSC:
        atom = new AP4_StscAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_STCO:
        atom = new AP4_StcoAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_CO64:
        atom = new AP4_Co64Atom(size, stream);
        break;

      case AP4_ATOM_TYPE_STSZ:
        atom = new AP4_StszAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_STTS:
        atom = new AP4_SttsAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_CTTS:
        atom = new AP4_CttsAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_STSS:
        atom = new AP4_StssAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_MP4S:
        atom = new AP4_Mp4sSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_MP4A:
		atom = parent && parent->GetType() == AP4_ATOM_TYPE_STSD 
			? (AP4_Atom*)new AP4_Mp4aSampleEntry(size, stream, *this)
			: (AP4_Atom*)new AP4_UnknownAtom(type, size, false, stream);
        break;

      case AP4_ATOM_TYPE_MP4V:
        atom = new AP4_Mp4vSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_AVC1:
        atom = new AP4_Avc1SampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_ENCA:
        atom = new AP4_EncaSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_ENCV:
        atom = new AP4_EncvSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_ESDS:
        atom = new AP4_EsdsAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_VMHD:
        atom = new AP4_VmhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_SMHD:
        atom = new AP4_SmhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_NMHD:
        atom = new AP4_NmhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_HMHD:
        atom = new AP4_HmhdAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_FRMA:
        atom = new AP4_FrmaAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_SCHM:
          atom = new AP4_SchmAtom(size, stream);
          break;

      case AP4_ATOM_TYPE_FTYP:
        atom = new AP4_FtypAtom(size, stream);
        break;
          
      case AP4_ATOM_TYPE_RTP:
        if (m_Context == AP4_ATOM_TYPE_HNTI) {
            atom = new AP4_RtpAtom(size, stream);
        } else {
            atom = new AP4_RtpHintSampleEntry(size, stream, *this);
        }
        break;
      
      case AP4_ATOM_TYPE_TIMS:
        atom = new AP4_TimsAtom(size, stream);
        break;
 
      case AP4_ATOM_TYPE_SDP:
        atom = new AP4_SdpAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_IKMS:
        atom = new AP4_IkmsAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_ISFM:
        atom = new AP4_IsfmAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_HINT:
        atom = new AP4_TrefTypeAtom(type, size, stream);
        break;

      // container atoms
      case AP4_ATOM_TYPE_TREF:
      case AP4_ATOM_TYPE_HNTI:
      case AP4_ATOM_TYPE_STBL:
      case AP4_ATOM_TYPE_MDIA:
      case AP4_ATOM_TYPE_DINF:
      case AP4_ATOM_TYPE_MINF:
      case AP4_ATOM_TYPE_SCHI:
      case AP4_ATOM_TYPE_SINF:
      case AP4_ATOM_TYPE_UDTA:
      case AP4_ATOM_TYPE_ILST:
	  case AP4_ATOM_TYPE_NAM:
	  case AP4_ATOM_TYPE_ART:
      case AP4_ATOM_TYPE_WRT:
      case AP4_ATOM_TYPE_ALB:
      case AP4_ATOM_TYPE_DAY:
      case AP4_ATOM_TYPE_TOO:
      case AP4_ATOM_TYPE_CMT:
      case AP4_ATOM_TYPE_GEN:
	  case AP4_ATOM_TYPE_TRKN:
	  case AP4_ATOM_TYPE_EDTS:
	  case AP4_ATOM_TYPE_WAVE: 
	  case AP4_ATOM_TYPE_CMOV: {
          AP4_UI32 context = m_Context;
          m_Context = type; // set the context for the children
          atom = new AP4_ContainerAtom(type, size, false, stream, *this);
          m_Context = context; // restore the previous context
          break;
      }

      // full container atoms
      case AP4_ATOM_TYPE_META:
        atom = new AP4_ContainerAtom(type, size, true, stream, *this);
        break;

      // other

      case AP4_ATOM_TYPE_AVCC:
        atom = new AP4_AvcCAtom(size, stream);
        break;

      case AP4_ATOM_TYPE_TEXT:
        atom = new AP4_TextSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_TX3G:
        atom = new AP4_Tx3gSampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_FTAB:
        atom = new AP4_FtabAtom(size, stream);
        break;

	  case AP4_ATOM_TYPE_CVID:
      case AP4_ATOM_TYPE_SVQ1:
      case AP4_ATOM_TYPE_SVQ2:
      case AP4_ATOM_TYPE_SVQ3:
	  case AP4_ATOM_TYPE_H263:
      case AP4_ATOM_TYPE_S263:
        atom = new AP4_VisualSampleEntry(type, size, stream, *this);
        break;

	  case AP4_ATOM_TYPE_SAMR:
	  case AP4_ATOM_TYPE__MP3:
	  case AP4_ATOM_TYPE_IMA4:
	  case AP4_ATOM_TYPE_QDMC:
	  case AP4_ATOM_TYPE_QDM2:
	  case AP4_ATOM_TYPE_TWOS:
	  case AP4_ATOM_TYPE_SOWT:
        atom = new AP4_AudioSampleEntry(type, size, stream, *this);
        break;

	  case AP4_ATOM_TYPE__AC3:
        atom = new AP4_AC3SampleEntry(size, stream, *this);
        break;

      case AP4_ATOM_TYPE_CHPL:
        atom = new AP4_ChplAtom(size, stream);
        break;

	  case AP4_ATOM_TYPE_DATA:
        atom = new AP4_DataAtom(size, stream);
        break;

	  case AP4_ATOM_TYPE_DCOM:
        atom = new AP4_DcomAtom(size, stream);
	    break;

	  case AP4_ATOM_TYPE_CMVD:
        atom = new AP4_CmvdAtom(size, stream, *this);
	    break;

      default:

		if(parent && parent->GetType() == AP4_ATOM_TYPE_STSD && (type & 0xffff0000) == AP4_ATOM_TYPE('m', 's', 0, 0))
		{
	        atom = new AP4_AudioSampleEntry(type, size, stream, *this);
		}
		else // try all the external type handlers
        {
            atom = NULL;
            AP4_List<TypeHandler>::Item* handler_item = m_TypeHandlers.FirstItem();
            while (handler_item) {
                TypeHandler* handler = handler_item->GetData();
                if (AP4_SUCCEEDED(handler->CreateAtom(type, size, stream, atom))) {
                    break;
                }
                handler_item = handler_item->GetNext();
            }
            if (atom == NULL) {
                // no custom handlers, create a generic atom
                atom = new AP4_UnknownAtom(type, size, false, stream);
            }
        }

		break;
    }

    // skip to the end of the atom
    bytes_available -= size;
    result = stream.Seek(start+size);
    if (AP4_FAILED(result)) {
        delete atom;
        atom = NULL;
    }

    return result;
}

