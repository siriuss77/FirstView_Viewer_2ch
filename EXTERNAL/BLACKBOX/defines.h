
enum _PlaybackState
{
	STATE_RUNNING = 0x00,
	STATE_PAUSED,
	STATE_STOPPED,
	STATE_CLOSED,
};

#define WM_USER_MEDIACHANGED WM_USER+88

typedef struct {
	DWORD dwItem1;    // dwItem1 contains the previous PIDL or name of the folder. 
	DWORD dwItem2;    // dwItem2 contains the new PIDL or name of the folder. 
} SHNOTIFYSTRUCT;


