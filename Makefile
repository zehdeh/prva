CPPFLAGS = -Wall -g
LDFLAGS = -lpq
OBJDIR = obj/
SRCDIR = src/
INCDIR = include/
OUTPUT = test

RM = rm -rf

SRCS = $(shell find $(SRCDIR) -name "*.cpp")
OBJS = $(addprefix $(OBJDIR),$(subst $(SRCDIR),,$(SRCS:.cpp=.o)))

all: psql

psql: $(OBJS)
	g++ $(OBJS) $(LDFLAGS) -o $(OUTPUT)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@mkdir -p $(@D)
	g++ $(CPPFLAGS) -I $(INCDIR) -c $< -o $@

clean:
	$(RM) $(OBJDIR)* $(OUTPUT)

clear: clean psql
