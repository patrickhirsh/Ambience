//
//  AddNodePage.swift
//  Ambience
//
//  Created by Patrick Hirsh on 6/3/21.
//

import SwiftUI

struct AddNode: View {
    @State private var ip: String = ""
    @State private var name: String = ""
    var AmbienceVM: AmbienceViewModel
    var body: some View {
        VStack
        {
            Text("Add Node")
            Form {
                Section {
                    TextField("IP", text: $ip)
                    TextField("Friendly Name", text: $name)
                }
                Section {
                    Button("Add Node") {
                        AmbienceVM.devices.append(AmbienceDevice(Name: name, IP: ip))
                    }
                }
                
            }
        }
    }
}
