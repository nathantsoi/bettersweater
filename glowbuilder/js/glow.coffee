class Glow
  constructor: ($parent) ->
    @$parent = $parent
    @$code = $parent.find('.code')

    # CONSTANTS
    # all coordinates are x,y
    @preview_size = [8,8]
    @editor_size = [64,8]
    @delay = 90

    # MEMBERS
    @cur_col = 0
    @$play_pause_btn = $parent.find('.play-pause')
    @pattern = []
    @preview = []
    @build()
    @bind()

    # populate the code box
    @updateCode()

    @play()

  build: ->
    @buildGrid(@$parent.find('.grid.editor'), @pattern, @editor_size)
    @buildGrid(@$parent.find('.grid.preview'), @preview, @preview_size)

  bind: ->
    self = @
    $('.play-pause').click(->
      self.togglePreview()
    )
    $('.editor .led').click(->
      self.togglePatternLed(@)
    )

  buildGrid: ($grid, data, coords) ->
    for y in [0...coords[1]]
      row = @buildDiv('row')
      $grid.append(row)
      for x in [0...coords[0]]
        if data.length - 1 < x
          data.push []
        if data[x].length - 1 < y
          data[x].push 0
        row.append(@buildDiv('cell').addClass('coord-' + x + '-' + y).append(@buildDiv('led')))

  buildDiv: (klass) ->
    $('<div class="' + klass + '"></div>')

  togglePatternLed: (led) ->
    $led = $(led)
    coord_xy = $led.parent('.cell').attr('class').match(/coord-(\d+)-(\d+)/i)
    x = parseInt coord_xy[1]
    y = parseInt coord_xy[2]
    $led.toggleClass('on')
    @pattern[x][y] = $led.is('.on')
    @updateCode()

  updateCode: ->
    @$parent.find('#x-size').val(@pattern.length)
    @$parent.find('#y-size').val(@pattern[0].length)
    code = ''
    rows = []
    debugger
    for y in [0...@pattern[0].length]
      rows.push []
      for x in [0...@pattern.length]
        rows[y][x] = if @pattern[x][y] then 1 else 0
      rows[y] = rows[y].join(',')
    code = rows.join('\n')
    #console.log 'pattern:\n', code
    @$code.val code

  togglePreview: ->
    if @interval
      @pause()
    else
      @play()

  pause: ->
    return unless @interval
    clearInterval(@interval)
    @interval = null
    @$play_pause_btn.text('play')
    @$play_pause_btn.addClass('running')

  play: ->
    return if @interval
    self = @
    @$play_pause_btn.text('pause')
    @$play_pause_btn.removeClass('running')
    @interval = setInterval(->
      self.render()
    , @delay)

  render: ->
    @cur_col = (@cur_col + 1) % @pattern.length
    #console.log 'render', @cur_col
    @$play_pause_btn.toggleClass('.on')
    for y in [0...@preview_size[1]]
      for x in [0...@preview_size[0]]
        $led = $('.preview .cell.coord-' + x + '-' + y).find('.led')
        if @pattern[(@cur_col + x) % @pattern.length][y]
          $led.addClass('on') if $led.is(':not(.on)')
        else
          $led.removeClass('on') if $led.is('.on')

$(->
  new Glow $('.glowtainer')
)