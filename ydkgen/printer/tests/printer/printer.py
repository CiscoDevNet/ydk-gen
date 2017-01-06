class Printer(object):
    def __init__(self, ctx, lang):
        self.ctx = ctx
        self.lang = lang

    def _write_end(self, line):
        if self.lang == 'py':
            self._writeln(line)
        elif self.lang == 'cpp':
            self._writeln('{};'.format(line))

    def _writeln(self, line):
        self.ctx.writeln(line)

    def _writelns(self, lines):
        self.ctx.writelns(lines)

    def _bline(self, num=1):
        while num > 0:
            self.ctx.bline()
            num -= 1

    def _write_comment(self, line):
        if self.lang == 'py':
            self.ctx.writeln('# {}'.format(line))
        elif self.lang == 'cpp':
            self.ctx.writeln('// {}'.format(line))

    def _write_comments(self, line):
        if self.lang == 'py':
            self.ctx.writeln('"""')
            self.ctx.writeln(line)
            self.ctx.writeln('"""')
        elif self.lang == 'cpp':
            self.ctx.writeln('/*')
            self.ctx.writeln(line)
            self.ctx.writeln('*/')

    def _lvl_inc(self):
        self.ctx.lvl_inc()

    def _lvl_dec(self):
        self.ctx.lvl_dec()
