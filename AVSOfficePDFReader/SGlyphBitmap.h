#ifndef _SGLYPH_BITMAP_H
#define _SGLYPH_BITMAP_H

//-------------------------------------------------------------------------------------------------------------------------------
// SGlyphBitmap
//-------------------------------------------------------------------------------------------------------------------------------

struct SGlyphBitmap 
{
	int            nX;        // ����� �� X ��������� ����� ��� ��������� �������
	int            nY;        // ����� �� Y ��������� ����� ��� ��������� �������
	int            nWidth;    // ������ �������
	int            nHeight;   // ������ �������
	BOOL           bAA;       // Anti-aliased: True ��������, ��� Bitmap 8-������(�.�. � ������); False - Bitmap 1-������
	unsigned char *pData;     // Bitmap data(�������� � ��������)
	BOOL           bFreeData; // True, ���� ������ � pData ����� ����������
};

#endif /* _SGLYPH_BITMAP_H */
