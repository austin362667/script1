# window.global = window

def log(arg):
    print(arg)


"""
export function str(o) {
    if (o.__str) return o.__str()
    return o.toString()
}

export function keys(o) {
    if (o instanceof Map || o instanceof Set) return Array.from(o.keys())
    return Object.keys(o)
}

export function values(o) {
    if (o instanceof Map || o instanceof Set) return Array.from(o.keys())
    return Object.keys(o)
}

export function range(from, to, step=1) {
    let list=[], i=from;
    while (i<to) {
        list.push(i);
        i+=step;
    }
    return list;
}

export function re(exp, attr) {
    let r = new RegExp(exp, attr)
    r.lastIndex = 0
    return r
}

global.log = log
global.str = str
global.keys = keys
global.values = values
global.range = range
global.re = re
"""