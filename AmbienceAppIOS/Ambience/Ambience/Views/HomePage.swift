//
//  ContentView.swift
//  Ambience
//
//  Created by Patrick Hirsh on 5/26/21.
//

import SwiftUI

struct HomePage: View {
    @ObservedObject var AmbienceVM: AmbienceViewModel
    
    var body: some View {
        NavigationView {
            VStack {
                NodeList(AmbienceVM: AmbienceVM)
                .navigationTitle(Text("Ambience Nodes"))
            }
        }
    }
}

struct NodeList: View {
    @ObservedObject var AmbienceVM: AmbienceViewModel
    var body: some View {
        List {
            ForEach(AmbienceVM.devices) { device in
                NavigationLink(destination: NodeSettings(AmbienceVM: AmbienceVM)) {
                    NodeEntry(IP: device.IP, Name: device.Name)
                }
            }
            NavigationLink(destination: AddNode(AmbienceVM: AmbienceVM)) {
                Label("Add Node", systemImage: "plus.circle")
                
                
            }
        }
    }
}

struct NodeEntry: View {
    var IP: String
    var Name: String
    var body: some View {
        VStack {
            HStack {
                Text(IP).bold()
                Spacer()
            }
            HStack {
                Text(Name)
                Spacer()
            }
        }
    }
}
