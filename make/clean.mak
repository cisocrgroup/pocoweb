clean:
	$(RM) $(ALL)
	$(RM) $(DEPS)
	$(RM) $(CORE_OBJS)
	$(RM) $(PARSER_OBJS)
	$(RM) $(API_OBJS)
	$(RM) $(PROFILER_OBJS)
	$(RM) $(PEX_OBJS)
	$(RM) $(PSLOG_OBJS)
	$(RM) $(PPROF_OBJS)
	$(RM) $(PLUGINS)
	$(RM) $(TESTS)
	$(RM) -r ext
	$(RM) -r lib

clean-cache:
	$(RM) make/cache.mak config.ini

clean-all: clean clean-cache

.PHONY: clean clean-cache clean-all
