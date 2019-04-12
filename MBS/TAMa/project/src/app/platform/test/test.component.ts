import { Component, OnInit } from '@angular/core';
import {FormBuilder, FormControl} from '@angular/forms';
import {Observable} from 'rxjs';
import {debounce, debounceTime, map, startWith, switchMap} from 'rxjs/operators';
import {GeocodingService} from '../../services/geocoding.service';
import {LocationIQAutocompleteItem} from '../../models/location-iqautocomplete';

@Component({
  selector: 'tp-test',
  templateUrl: './test.component.html',
  styleUrls: ['./test.component.scss']
})
export class TestComponent {

  usersForm = this.fb.group({
    userInput: null
  });
  filteredUsers =
    this.usersForm
      .get('userInput')
      .valueChanges
      .pipe(
        debounceTime(300),
        switchMap(value => this.geocode.autocomplete(value).pipe(map(x => {console.log(x); return x;})))
      );

  myControl = new FormControl();

  constructor(private readonly geocode: GeocodingService,
              private readonly fb: FormBuilder) {
  }
}
