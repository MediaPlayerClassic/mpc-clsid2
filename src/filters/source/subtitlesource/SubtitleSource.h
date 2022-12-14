/* 
 *	Copyright (C) 2003-2006 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *   
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *   
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. 
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include <atlbase.h>
#include "..\..\..\subtitles\RTS.h"

class CSubtitleSource
	: public CSource
	, public IFileSourceFilter
	, public IAMFilterMiscFlags
{
protected:
	CStringW m_fn;

public:
	CSubtitleSource(LPUNKNOWN lpunk, HRESULT* phr, const CLSID& clsid);
	virtual ~CSubtitleSource();

	DECLARE_IUNKNOWN;
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	// IFileSourceFilter
	STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE* pmt);
	STDMETHODIMP GetCurFile(LPOLESTR* ppszFileName, AM_MEDIA_TYPE* pmt);

	// IAMFilterMiscFlags
	STDMETHODIMP_(ULONG) GetMiscFlags();

    virtual HRESULT GetMediaType(CMediaType* pmt) = 0;
};

class CSubtitleStream 
	: public CSourceStream
	, public CSourceSeeking
{
	CCritSec m_cSharedState;

	int m_nPosition;

	BOOL m_bDiscontinuity, m_bFlushing;

	HRESULT OnThreadStartPlay();
	HRESULT OnThreadCreate();

	void UpdateFromSeek();
	STDMETHODIMP SetRate(double dRate);

	HRESULT ChangeStart();
    HRESULT ChangeStop();
    HRESULT ChangeRate() {return S_OK;}

protected:
	CRenderedTextSubtitle m_rts;

public:
    CSubtitleStream(const WCHAR* wfn, CSubtitleSource* pParent, HRESULT* phr);
	virtual ~CSubtitleStream();

    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

    HRESULT DecideBufferSize(IMemAllocator* pIMemAlloc, ALLOCATOR_PROPERTIES* pProperties);
    HRESULT FillBuffer(IMediaSample* pSample);
    HRESULT GetMediaType(CMediaType* pmt);
	HRESULT CheckMediaType(const CMediaType* pmt);

	STDMETHODIMP Notify(IBaseFilter* pSender, Quality q);
};

[uuid("E44CA3B5-A0FF-41A0-AF16-42429B1095EA")]
class CSubtitleSourceASCII : public CSubtitleSource
{
public:
	CSubtitleSourceASCII(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("87864E0F-7073-4E39-B802-143DE0ED4964")]
class CSubtitleSourceUTF8 : public CSubtitleSource
{
public:
	CSubtitleSourceUTF8(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("18316B1A-5877-4CC4-85FD-EDE65CD489EC")]
class CSubtitleSourceSSA : public CSubtitleSource
{
public:
	CSubtitleSourceSSA(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("416782BC-1D87-48C0-8F65-F113A5CB8E15")]
class CSubtitleSourceASS : public CSubtitleSource
{
public:
	CSubtitleSourceASS(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("D7215AFC-DFE6-483B-9AF3-6BBECFF14CF4")]
class CSubtitleSourceUSF : public CSubtitleSource
{
public:
	CSubtitleSourceUSF(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("932E75D4-BBD4-4A0F-9071-6728FBDC4C98")]
class CSubtitleSourcePreview : public CSubtitleSource
{
public:
	CSubtitleSourcePreview(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

[uuid("CF0D7280-527D-415E-BA02-56017484D73E")]
class CSubtitleSourceARGB : public CSubtitleSource
{
public:
	CSubtitleSourceARGB(LPUNKNOWN lpunk, HRESULT* phr);

	HRESULT GetMediaType(CMediaType* pmt);
};

