# =============================================================================
# Variables
# =============================================================================

CC 		= dpu-upmem-dpurte-clang
LD 		= dpu-upmem-dpurte-clang

CFLAGS 	= -Wall -Wextra -Wno-unused-label -Wno-unused-function -O3

TM 		= tiny
TMLIB 	= lib$(TM).a

SRCS	= tiny.c
OBJS	= ${SRCS:.c=.o}

RM		= rm -f

# =============================================================================
# Defines
# =============================================================================

ifdef TX_IN_MRAM
	DEFINES += -DTX_IN_MRAM
endif

ifdef DATA_IN_MRAM
	DEFINES += -DDATA_IN_MRAM
endif

ifdef BACKOFF
	DEFINES += -DBACKOFF
endif

ifdef WRITE_BACK_CTL
	DEFINES += -DWRITE_BACK_CTL
else
	ifdef WRITE_BACK_ETL
		DEFINES += -DWRITE_BACK_ETL
	else
		DEFINES += -DWRITE_THROUGH_ETL
	endif
endif

ifdef R_SET_SIZE
	DEFINES += -DR_SET_SIZE=$(R_SET_SIZE)
endif

ifdef W_SET_SIZE
	DEFINES += -DW_SET_SIZE=$(W_SET_SIZE)
endif

ifdef LOCK_ARRAY_LOG_SIZE
	DEFINES += -DLOCK_ARRAY_LOG_SIZE=$(LOCK_ARRAY_LOG_SIZE)
endif


# =============================================================================
# Rules
# =============================================================================

.PHONY: all clean
all: $(TMLIB)

%.o: %.c
	$(CC) $(CFLAGS) $(DEFINES) -c $< -o $@

$(TMLIB): $(OBJS)
	$(AR) crus $@ $^

clean:
	$(RM) $(TMLIB) $(OBJS)

# gcc -I/home/andre/Documents/PIM_tinySTM/src 
# -o prog main.c -L/home/andre/Documents/PIM_tinySTM/lib/ -l tiny