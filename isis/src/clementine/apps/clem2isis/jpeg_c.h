
/* Structure Definitions */
enum FmodeDef { INPUT, OUTPUT };
typedef enum FmodeDef Fmode;

struct ByteStreamDef
{
	Fmode   mode;
	short   stat;
	FILE    *file;
};
typedef struct ByteStreamDef ByteStream;

extern void *cByteStream( ByteStream *, char *FileName, Fmode FileMode );
extern void *dByteStream(ByteStream *);
extern short ByteStream_read(ByteStream *);
extern short ByteStream_write(ByteStream *, short c);
extern short ByteStream_status(ByteStream *);

#define MEMORY  1
#define DISK    0
#if defined(__BORLANDC__) && !defined(__WIN32__)
#define MALLOC	farmalloc
#define FREE	farfree
#define CHARH	unsigned char huge
#define FAR		far
#else
#define MALLOC	malloc
#define FREE	free
#define CHARH	unsigned char
#define FAR
#endif

struct BitStreamDef
{
	ByteStream	bytestream;
	short		BitBuffer;      /* Bit I/O buffer */
	short     	BitBuffMask;    /* Bit I/O buffer mask */
	CHARH		*outstring;
	char    mode;
	unsigned long	bytesout;
	unsigned short	bitmask[17];
};
typedef struct BitStreamDef BitStream;

extern void *cBitStream( BitStream *, char *fn, Fmode fm );
extern void *dBitStream(BitStream *);
extern short BitStream_write(BitStream *, short bits, short width);
extern short BitStream_read(BitStream *, short bits);

/* Global Tables */
extern float	q_table[64];
extern int	zzseq[64];
extern short	dcbits[16], acbits[16];
extern char	dchuffval[12], achuffval[162];

/* Function Declarations */
extern void inithuffcode();
extern void encode(short *, BitStream *);
extern void decode(short *, BitStream *);
extern void decomp(BitStream *bs,CHARH *Image,long rows,long cols);
