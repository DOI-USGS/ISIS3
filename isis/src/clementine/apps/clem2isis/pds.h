
#if defined(__BORLANDC__) && !defined(__WIN32__)
#define CHARH	unsigned char huge
#else
#define CHARH unsigned char
#endif

typedef struct {
	char    	*text;		/* pointer to text header */
	long    	*hist;      /* pointer to histogram   */
	unsigned char   *brw_imag;      /* pointer to browse image (if available) */
	int		browse_nrows;   /* number of rows in browse image */
    int		browse_ncols;   /* number of columns in browse image */
	CHARH   *image;         /* pointer to decompressed or uncompressed image */
	int		image_nrows;    /* number of rows in the image */
    int		image_ncols;    /* number of columns in the image */
} PDSINFO;

//extern FILE     *qparm;          //removed qparm references BMG 2006-07-18

extern PDSINFO *PDSR(char *fname, long *rows, long *cols);
