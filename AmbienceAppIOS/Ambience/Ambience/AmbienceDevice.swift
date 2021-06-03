//
//  AmbienceDevice.swift
//  Ambience
//
//  Created by Patrick Hirsh on 6/2/21.
//

import Foundation

class AmbienceDevice {
    private(set) var Name: String
    private(set) var IP: String
    
    init(Name name: String, IP ip: String) {
        Name = name
        IP = ip
    }
    
    func SetMode(_ mode: String) {
        // URLSession: https://stackoverflow.com/questions/24016142/how-do-i-make-an-http-request-in-swift
        let url = URL(string: IP + "/SetMode?Mode=" + mode)
        let task = URLSession.shared.dataTask(with: url!) {(data, response, error) in
            guard let data = data else { return }
            print(String(data: data, encoding: .utf8)!)
        }
        task.resume()
    }
    
    func SetColor1(Hue h: Int, Saturation s: Int, Value v: Int) { SetColor(1, h, s, v)}
    func SetColor2(Hue h: Int, Saturation s: Int, Value v: Int) { SetColor(2, h, s, v)}
    func SetColor3(Hue h: Int, Saturation s: Int, Value v: Int) { SetColor(3, h, s, v)}
    
    private func SetColor(_ c: Int, _ h: Int, _ s: Int, _ v: Int) {
        // URLSession: https://stackoverflow.com/questions/24016142/how-do-i-make-an-http-request-in-swift
        let url = URL(string: IP + "/SetColor" + String(c) + "?h=" + String(h) + "&s=" + String(s) + "&v=" + String(v))
        let task = URLSession.shared.dataTask(with: url!) {(data, response, error) in
            guard let data = data else { return }
            print(String(data: data, encoding: .utf8)!)
        }
        task.resume()
    }
}
