NAME = view_tester

FBT = ./fbt

all: run

run:
	@cd ../.. && \
		$(FBT) launch_app APPSRC=applications/$(NAME)
