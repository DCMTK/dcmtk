#!/bin/sh
# the next line restarts using wish \
exec wish4.2 "$0" "$@"

# inserts the tag value

proc insertTagValue {w s} {
    set sp [string index $s 0]
    if {$sp == "("} {
      if {[string match {*\,*} $s] == 0} {
        $w insert end $s tag_val1        
      } else {
        $w insert end $s tag_val2
      }
    } elseif {$sp == "\["} {
        $w insert end $s tag_val2
    } elseif {$sp == "="} {
        $w insert end $s tag_val3
    } else {
        $w insert end $s tag_val4
    }
}


# load text from file into window

proc loadText {w name} {
#    $w configure -state normal
    set f [open $name]
    while {![eof $f]} {
        gets $f s
        set sp [string index $s 0]
        if {$sp == "#" || $sp == "*"} {
            $w insert end $s tag_emph
        } elseif {[string match {(\ )*\(*\)*\#*} $s] == 0} {
            set sp1 [string first ")" $s]
            $w insert end [string range $s 0 $sp1] tag_tag
            set sp1 [expr int ($sp1 + 1)]
            set sp2 [expr int ($sp1 + 3)]
            $w insert end [string range $s $sp1 $sp2] tag_vr
            set sp3 [string last "#" $s]
            set sp1 [expr int ($sp2 + 1)]
            set sp2 [expr int ($sp3 - 1)]
            insertTagValue $w [string range $s $sp1 $sp2]
            set ss [string range $s $sp3 end]
            set sp3 [string last " " $ss]
            set sp2 [expr int ($sp3)]
            $w insert end [string range $ss 0 $sp2] tag_len
            set sp1 [expr int ($sp3 + 1)]
            $w insert end [string range $ss $sp1 end] tag_name
        } else {
            $w insert end $s
        }
        $w insert end "\n"
    }
    close $f
#    $w configure -state disabled
}


# search string in text window and mark all positions with tag

proc findText {w name tag} {
    $w tag remove $tag 1.0 end
    set hits 0
    set pos -1
    if {$name != ""} {
        set cur 1.0
        while 1 {
            set cur [$w search -count length -- $name $cur end]
            if {$cur == ""} {
                break
            }
            if {$pos < 0} {
                set pos $cur
            }
            $w tag add $tag $cur "$cur + $length char"
            set cur [$w index "$cur + $length char"]
            incr hits
        }
    }
    if {$pos >= 0} {
        $w see $pos
    }
    return $hits
}

# search string in both text windows

proc findTextCommand {name} {
    global hitsText

    set dh [findText .dump.text $name tag_found]
    set hitsText [concat "Hits: " $dh]
    focus .find.entry
    .find.entry delete 0 end
}


# set up text window

proc setupTextWindow {} {
    global findName
    global hitsText
    
    set hitsText "Hits: 0"
 
    frame .find
    button .find.button -text "Find" -command "findTextCommand  \$findName" -width 4 -anchor w
    entry .find.entry -textvariable findName
    label .find.label -textvariable hitsText -width 13
    pack .find.button -side left
    pack .find.label -side right 
    pack .find.entry -fill x -expand yes -padx 5

    frame .dump
    text .dump.text -height 1 -wrap none -background #fff7e9 -selectbackground lightgray -font -*-Courier-Medium-R-Normal--*-150-*-*-*-*-*-* -xscrollcommand ".dump.xscroll set" -yscrollcommand ".dump.yscroll set"
    scrollbar .dump.xscroll -orient horizontal -command ".dump.text xview"
    scrollbar .dump.yscroll -orient vertical -command ".dump.text yview"
    pack .dump.xscroll -side bottom -fill x
    pack .dump.yscroll -side right -fill y
    pack .dump.text -expand yes -fill both

    
    .dump.text tag configure tag_emph -foreground black -font -*-Courier-Bold-R-Normal--*-150-*-*-*-*-*-*
    .dump.text tag configure tag_found -background lightgray -underline true
    .dump.text tag configure tag_tag -foreground darkblue
    .dump.text tag configure tag_vr -foreground darkred
    .dump.text tag configure tag_val1 -foreground darkgray
    .dump.text tag configure tag_val2 -foreground black
    .dump.text tag configure tag_val3 -foreground darkblue
    .dump.text tag configure tag_val4 -foreground black
    .dump.text tag configure tag_len -foreground darkgray
    .dump.text tag configure tag_name -foreground darkblue

    pack .find -side top -fill x
    pack .dump -expand yes -fill both

    bind .find.entry <Return> "findTextCommand  \$findName"
    bind .dump.text <Button-3> {if {[.dump.text tag nextrange sel 1.0] != ""} {findTextCommand [selection get]}}
    
    focus .find.entry
}


# main window
catch {eval destroy [winfo child .]} errMsg
if {$errMsg > 0} {
   exit 1
}

# initialization
set temp "/tmp"
if [info exists env(TEMP)] {
   set temp $env(TEMP)
} elseif [info exists env(TMP)] {
   set temp $env(TMP)
}
set dumpReport  [file join $temp dcmdump.report]
set dumpApp "dcmdump"

# set up window
wm iconname . "DICOM Dump"
if {$tcl_platform(os) == "SunOS"} {
   wm geometry . 956x903
} else {
   wm geometry . [expr int([winfo screenwidth .] * 0.995)]x[expr int([winfo screenheight .] * 0.938)]
}
wm geometry . +0+0
wm minsize . 400 165

setupTextWindow

# set up bindings
bind all <Control-c> {destroy .}

# start dump process
if {$argc > 0} {
  set fileName [lindex $argv 0]
  wm title . "Please wait ..."
  catch {exec $dumpApp $fileName >& $dumpReport}
  loadText .dump.text $dumpReport
  update
  wm title . [format "DICOM File Contents  -  %s" $fileName]
} else {
  wm title . [format "DICOM File Contents  -  <no filename specified>"]
}
