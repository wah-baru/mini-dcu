# project name (generate executable with this name)
TARGET   = gurux.dlms.datacollector.bin

CC       = gcc

# compiling flags here
CFLAGS   = -std=gnu99 -Wall -I. -DDLMS_IGNORE_MALLOC -DDLMS_USE_EPOCH_TIME

LINKER   = gcc -o

# linking flags here
LFLAGS   = -L../development/lib 

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS) -lgurux_dlms_c -lpthread -lm -lpaho-mqtt3c -lpaho-mqtt3a
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!" 
	@echo $(OBJECTS)

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

# # DLMS_SRCDIR = dlms/src
# SRCDIR      = src
# OBJDIR      = obj
# BINDIR      = bin

# DLMS_SOURCES  := $(wildcard $(DLMS_SRCDIR)/*.c)
# SOURCES  := $(wildcard $(SRCDIR)/*.c)
# $(info $(SOURCES))
# $(info $(DLMS_SOURCES))

# OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# DLMS_OBJECTS  := $(DLMS_SOURCES:$(DLMS_SRCDIR)/%.c=$(OBJDIR)/%.o)
# rm       = rm -f

# $(BINDIR)/$(TARGET): $(OBJECTS) $(DLMS_OBJECTS)
# 	@$(LINKER) $@ $(OBJECTS) $(DLMS_OBJECTS) -lpthread -lm
# 	@echo "Linking complete!"

# $(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
# 	@$(CC) $(CFLAGS) -c $< -o $@
# 	@echo "Compiled "$<" successfully!"

# $(DLMS_OBJECTS): $(OBJDIR)/%.o : $(DLMS_SRCDIR)/%.c
# 	@$(CC) $(CFLAGS) -c $< -o $@
# 	@echo "Compiled "$<" successfully!"

# .PHONEY: clean
# clean:
# 	@$(rm) $(OBJECTS)
# 	@$(rm) $(DLMS_OBJECTS)
# 	@echo "Cleanup complete!"
# 	@echo $(OBJECTS)

# .PHONEY: remove
# remove: clean
# 	@$(rm) $(BINDIR)/$(TARGET)
# 	@echo "Executable removed!"